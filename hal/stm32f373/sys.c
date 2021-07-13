/**
 * @file sys.c
 *
 * @brief implements the sys module of mos for the stm32f107x cpu
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */


#include <stm32f37x_conf.h>
#include "hal.h"


/* internal structure used to store system states etc so they are all in
 * one easy place to find. */
struct SYS_T
{
	volatile uint32_t ticks;
	volatile int32_t critical_section_count;
	enum SYS_ERR error;
};
static struct SYS_T sys = {0,};


// setup all the system clocks
#define SYS_CLK 72000000
static void sys_clk_init(void)
{
	ErrorStatus HSEStartUpStatus;

	// set clocks registers back to defaults (for debugging, st_demo)
	RCC_DeInit();

	// enable the high speed external osc (HSE) and spin for it to stabilise
	RCC_HSEConfig(RCC_HSE_ON);
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	if (HSEStartUpStatus != SUCCESS)
	{
		///@todo handle error better
		while (1)
		{}
	}

	// Enable Prefetch Buffer
	FLASH_PrefetchBufferCmd(ENABLE);

	// Flash 1 wait state
	FLASH_SetLatency(FLASH_Latency_2);

	// AHB prescaler set to div 1, HCLK = SYSCLK (72MHz)
	RCC_HCLKConfig(RCC_SYSCLK_Div1);

	// APB2 (high speed) prescaler set to div 1, APB2 = HCLK
	RCC_PCLK2Config(RCC_HCLK_Div1);

	// APB1 (low speed) prescaler set to div 2, APB1 = HCLK / 2
	RCC_PCLK1Config(RCC_HCLK_Div2);

	// Setup the PLLCLK source and pre scaler (8MHz * 9 = 72MHz)
	///@note if you change this update the SYS_CLK definition
	RCC_PREDIV1Config(RCC_CFGR2_PREDIV1_DIV1);
	RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);

	// Enable the PLL and spin for it to be ready
	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{}

	// Switch the system clock over to the PLL output and spin until it is ready
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while(RCC_GetSYSCLKSource() != 0x08) ;

	// enable backup register domain clocks incase they are not enabled
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div32);
	RCC_RTCCLKCmd(ENABLE);

#ifdef DEBUG_MCO
	/* this is a handy debugging option to output the sys_clk/2 to the MCO p
	 * it must be sys_clk/2 as gpio can only output 50MHz max, so 72MHz/2 = 36MHz */
	RCC_MCOConfig(RCC_MCOSource_PLLCLK_Div2);
#endif
}


// init system interrupts
static void sys_interrupt_init(void)
{
	///@todo stub for now (we should at least call NVIC_PriorityGroupConfig() here
}


// init a log via uart or trace
static void sys_log_init(void)
{
	///@todo uart log
}


// disable all interrupts and inc the counter so this can be re-entrant
void sys_enter_critical_section(void)
{
	__disable_irq();
	sys.critical_section_count++;

	// error checking (have we rolled over or something
	if (sys.critical_section_count < 0)
	{
		///@todo log error !
		sys.critical_section_count = 1;
	}
}


// decrement the critical section counter and if it hits 0 re-enable interrupts
void sys_leave_critical_section(void)
{
	sys.critical_section_count--;
	if (sys.critical_section_count <= 0)
	{
		__enable_irq();

		// in case count get out of sync, log an error, and try to recover by resetting
		if (sys.critical_section_count < 0)
		{
			///@todo log error !
			sys.critical_section_count = 0;
		}
	}
}


// get the system clock speed in Hz
uint32_t sys_clk_freq(void)
{
	return SYS_CLK;
}


// setup the tick handler interrupt rate
static void sys_tick_init()
{
	if (SysTick_Config(SYS_CLK / 1000) != 0)
	{}

	NVIC_SetPriority(SysTick_IRQn, 2);
}


// sys tick ISR (overrides weak functions from st libs)
void SysTick_Handler(void)
{
	sys.ticks++;
}


// get the current system time with ms resolution, use a shadow to protect against interrupts
uint32_t sys_get_tick(void)
{
	uint32_t tick_shadow;

	sys_enter_critical_section();
	tick_shadow = sys.ticks;
	sys_leave_critical_section();

	return(tick_shadow);
}


// determine the number of ticks between end and beginning
uint32_t sys_abs_tick_diff(uint32_t beginning, uint32_t end)
{
	if (end >= beginning)
		// standard case 
		// |----b--------------e----|
		//      <-------------->     UINT32_MAX
		return end - beginning;
	else
		// wrapped case
		// |----e--------------b----|
		// <---->      +       <---->UINT32_MAX
		return (UINT32_MAX - beginning) + end;
}

// determine the number of tick t1 is away from t2 (negative if t2 is earlier than t1)
int32_t sys_tick_diff(uint32_t t1, uint32_t t2)
{
	uint32_t tdiff1 = sys_abs_tick_diff(t1, t2);
	uint32_t tdiff2 = sys_abs_tick_diff(t2, t1);

	if (tdiff1 < tdiff2)
		// t1 is likely earlier than t2, otherwise the time difference is very large
		//                  t1       t2
		// |-----------------x--------x-----|
		//                   ----------        < tdiff1, more likely as the two times are closer together
		//  -----------------          -----   < tdiff2
		return tdiff1; // positive as t2 is in the future relative to t1
	else
		// t2 is likely later than t1, otherwise the time difference is very large
		//                  t2       t1
		// |-----------------x--------x-----|
		//  -----------------          -----   < tdiff1
		//                   ----------        < tdiff2, more likely as the two times are closer together
		return -tdiff2; // negative as t2 is in the past relative to t1
}


typedef void (*sys_task)(uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3);
void sys_run(void *func, uint8_t argc, uint32_t argv[])
{
	sys_task t;
	if (!func)
		return;
	if (argc > 4)
		///@todo error out here we only support 4 params atm
		return;

	// atm just support 4 params (this is easy as the call below just 
	// puts the 4 param into R0..R3, if we want more then they will need
	// to be pushed onto the stack which is much harder as the types matter
	// then)
	t = (sys_task)func;
	t(argv[0], argv[1], argv[2], argv[3]);
}


// soft reset the whole cpu
void sys_reset(void)
{
	NVIC_SystemReset();
}


///@todo need a prototype for printf
void sys_log()
{
}


void BusFault_Handler(void)
{
	//if (CoreDebug->DHCSR & 0x01)		//is C_DEBUGEN set, is the debugger connected?
	{
		//__breakpoint(0);
		while (1) {};
	}

}

/**
 * @brief Hard Fault ISR (a hard fault has occurred, log a message and spin)
 */
void HardFault_Handler(void)
{
	//if (CoreDebug->DHCSR & 0x01)		//is C_DEBUGEN set, is the debugger connected?
	{
		//__breakpoint(0);
		while (1) {};
	}
}


// get the last error logged at the system level
enum SYS_ERR sys_get_error(void)
{
	enum SYS_ERR sys_error_shadow;

	sys_enter_critical_section();
	sys_error_shadow = sys.error;
	sys_leave_critical_section();

	return sys_error_shadow;
}


void sys_nop(void)
{
	__NOP();
}


// spin for time ms
void sys_spin(uint32_t time)
{
	uint32_t end_time = sys_get_tick() + time;

	while (end_time != sys_get_tick())
	{}
}


// setup the basic components of any system
void sys_init(void)
{
	sys_clk_init();
	sys_interrupt_init();
	sys_tick_init();
	sys_log_init();
}


#ifdef USE_FULL_ASSERT
/**
 * @brief report the file and line where the assert failed and spin
 * @param  file pointer to the source file name
 * @param  line assert_param error line source number
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* Infinite loop */
	while (1)
	{}
}

#endif



