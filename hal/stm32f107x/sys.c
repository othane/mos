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


#include <stm32f10x_conf.h>
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
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

	// Flash 1 wait state
	FLASH_SetLatency(FLASH_Latency_2);

	// AHB prescaler set to div 1, HCLK = SYSCLK (72MHz)
	RCC_HCLKConfig(RCC_SYSCLK_Div1);

	// APB2 (high speed) prescaler set to div 1, APB2 = HCLK
	RCC_PCLK2Config(RCC_HCLK_Div1);

	// APB1 (low speed) prescaler set to div 2, APB1 = HCLK / 2
	RCC_PCLK1Config(RCC_HCLK_Div2);

	// Setup the PLLCLK source and pre scaler (25MHz * 5 / 2 = 62MHz)
	///@note if you change this update the SYS_CLK definition
	RCC_PREDIV1Config(RCC_PREDIV1_Source_HSE, RCC_PREDIV1_Div2);
	RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_5);

	// Enable the PLL and spin for it to be ready
	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{}

	// Switch the system clock over to the PLL output and spin until it is ready
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while(RCC_GetSYSCLKSource() != 0x08) ;

#ifdef DEBUG_MCO
	/* this is a handy debugging option to output the sys_clk/2 to the MCO p
	 * it must be sys_clk/2 as gpio can only output 50MHz max, so 72MHz/2 = 36MHz */
	RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);
#endif
}


// init system interrupts
static void sys_interrupt_init(void)
{
	///@todo stub for now (we should at least call NVIC_PriorityGroupConfig() here
}


// init the on chip temperature sensor
static void sys_temp_init(void)
{
	///@todo this is just a stub for now
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
	sys_temp_init();
	sys_log_init();

	// i dont know what is going on here (something HW related), but it seems to
	// be needed for the blade layout, I will look at it when I have more time.
	USART_DeInit(USART3);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO,ENABLE);

	// for some reason we need to spin here (I dont know why, maybe a bad clk)
	sys_spin(100);
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



