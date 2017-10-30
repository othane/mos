/**
 * @file tmr.c
 *
 * @brief implement the timer module for the stm32f373
 *
 * this is a very basic timer base module that allows setup 
 * of the time base unit only, for functional things like pwm
 * and capture compare events see the specific module
 *
 * @author OT
 *
 * @date Apr 2015
 *
 */


#include <stm32f37x_conf.h>
#include <math.h>
#include <float.h>
#include "hal.h"
#include "tmr_hw.h"

#define MIN(a,b) ((a < b)? a: b)
#define MAX(a,b) ((a > b)? a: b)
#define CLIP(a, amin, amax) MAX((MIN(a, amax)), amin)


// look up the irq channel for this tmr (18 seems a little wasteful, but it is easier atm)
static tmr_t *tmr_irq_list[18] = {NULL,};  ///< just store the tmr handle so we can get it in the irq (then hw.c is more free form)
static uint8_t tmr_irq(tmr_t *tmr)
{
	switch ((uint32_t)tmr->tim)
	{
		case (uint32_t)TIM2:
			tmr_irq_list[2] = tmr;
			return TIM2_IRQn;
		case (uint32_t)TIM3:
			tmr_irq_list[3] = tmr;
			return TIM3_IRQn;
		case (uint32_t)TIM4:
			tmr_irq_list[4] = tmr;
			return TIM4_IRQn;
		case (uint32_t)TIM5:
			tmr_irq_list[5] = tmr;
			return TIM5_IRQn;
		case (uint32_t)TIM6:
			tmr_irq_list[6] = tmr;
			return TIM6_DAC1_IRQn;
		case (uint32_t)TIM7:
			tmr_irq_list[7] = tmr;
			return TIM7_IRQn;
		case (uint32_t)TIM12:
			tmr_irq_list[12] = tmr;
			return TIM12_IRQn;
		case (uint32_t)TIM13:
			tmr_irq_list[13] = tmr;
			return TIM13_IRQn;
		case (uint32_t)TIM14:
			tmr_irq_list[14] = tmr;
			return TIM14_IRQn;
		case (uint32_t)TIM18:
			tmr_irq_list[18] = tmr;
			return TIM18_DAC2_IRQn;
		default:
			///@todo error
			return 0x00;
	}
}

static void init_rcc(struct tmr_t *tmr)
{
	switch ((int)tmr->tim)
	{
		// APB1 -- slower timers (fsys / 2)
		case (int)TIM2:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
			break;
		case (int)TIM3:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
			break;
		case (int)TIM4:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
			break;
		case (int)TIM5:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
			break;
		case (int)TIM6:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
			break;
		case (int)TIM7:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
			break;
		case (int)TIM12:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
			break;
		case (int)TIM13:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
			break;
		case (int)TIM14:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
			break;
		case (int)TIM18:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM18, ENABLE);
			break;

		// APB2 -- full speed timers
		case (int)TIM15:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
			break;
		case (int)TIM16:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
			break;
		case (int)TIM17:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);
			break;
		case (int)TIM19:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM19, ENABLE);
			break;

		default:
			///@todo warn timer is not valid
			break;
	}
}


static void dbg_stop(struct tmr_t *tmr)
{
	int state = tmr->stop_on_halt;
	switch ((int)tmr->tim)
	{
		case (int)TIM2:
			DBGMCU_APB1PeriphConfig(DBGMCU_TIM2_STOP, state);
			break;
		case (int)TIM3:
			DBGMCU_APB1PeriphConfig(DBGMCU_TIM3_STOP, state);
			break;
		case (int)TIM4:
			DBGMCU_APB1PeriphConfig(DBGMCU_TIM4_STOP, state);
			break;
		case (int)TIM5:
			DBGMCU_APB1PeriphConfig(DBGMCU_TIM5_STOP, state);
			break;
		case (int)TIM6:
			DBGMCU_APB1PeriphConfig(DBGMCU_TIM6_STOP, state);
			break;
		case (int)TIM7:
			DBGMCU_APB1PeriphConfig(DBGMCU_TIM7_STOP, state);
			break;
		case (int)TIM12:
			DBGMCU_APB1PeriphConfig(DBGMCU_TIM12_STOP, state);
			break;
		case (int)TIM13:
			DBGMCU_APB1PeriphConfig(DBGMCU_TIM13_STOP, state);
			break;
		case (int)TIM14:
			DBGMCU_APB1PeriphConfig(DBGMCU_TIM14_STOP, state);
			break;
		case (int)TIM18:
			DBGMCU_APB1PeriphConfig(DBGMCU_TIM18_STOP, state);
			break;

		case (int)TIM15:
			DBGMCU_APB2PeriphConfig(DBGMCU_TIM15_STOP, state);
			break;
		case (int)TIM16:
			DBGMCU_APB2PeriphConfig(DBGMCU_TIM16_STOP, state);
			break;
		case (int)TIM17:
			DBGMCU_APB2PeriphConfig(DBGMCU_TIM17_STOP, state);
			break;
		case (int)TIM19:
			DBGMCU_APB2PeriphConfig(DBGMCU_TIM19_STOP, state);
			break;

		default:
			///@todo warn timer is not valid
			break;
	}
}


static uint32_t get_tmr_freq(struct tmr_t *tmr)
{
	if (tmr->sync.ext_clk_mode)
		return tmr->sync.ext_clk_freq;
	else
		// the timers on the APB1 bus run at the same speed as those on the
		// APB2 bus since the APB1 prescaler is 2 (see ref manual p98)
		return sys_clk_freq();
}


void tmr_sync_cfg(struct tmr_t *tmr, uint8_t ext_clk_mode, uint8_t sync_mode)
{
	uint16_t slave_mode = 0;

	if (sync_mode)
		slave_mode = tmr->sync.slave_mode;

	switch (ext_clk_mode)
	{
		case 0:
			// run from the timers internal clk
			tmr->tim->SMCR &= ~TIM_SMCR_ECE;
			break;
		case 1:
			// clock is derived from TRGI (SMS = 111, ECE = 0)
			tmr->tim->SMCR &= ~TIM_SMCR_ECE;
			slave_mode = TIM_SlaveMode_External1;
			break;
		case 2:
			// clock is derived from ETRF (ECE = 1, TS!=ETRF)
			tmr->tim->SMCR |= TIM_SMCR_ECE;
			break;
	}
	tmr->sync.ext_clk_mode = ext_clk_mode;

	TIM_SelectSlaveMode(tmr->tim, slave_mode); // set SMS bits
	tmr->sync.slave_mode = slave_mode;
}


void tmr_start(tmr_t *tmr)
{
	TIM_ClearITPendingBit(tmr->tim, TIM_IT_Update);
	TIM_ITConfig(tmr->tim, TIM_IT_Update, ENABLE);
	TIM_Cmd(tmr->tim, ENABLE);
}


void tmr_stop(tmr_t *tmr)
{
	TIM_ITConfig(tmr->tim, TIM_IT_Update, DISABLE);
	TIM_ClearITPendingBit(tmr->tim, TIM_IT_Update);
	TIM_Cmd(tmr->tim, DISABLE);
}


int tmr_running(tmr_t *tmr)
{
	return (tmr->tim->CR1 & TIM_CR1_CEN? 1: 0);
}


void tmr_reset(tmr_t *tmr)
{
	tmr->tim->CNT = 0;
}


static uint8_t tmr_ch2n(int ch)
{
	switch (ch)
	{
		case TIM_Channel_1:
			return 0;
		case TIM_Channel_2:
			return 1;
		case TIM_Channel_3:
			return 2;
		case TIM_Channel_4:
			return 3;
		default:
			return 0;	// error, this should not happen !!
	}
}


static uint8_t tmr_n2ch(int n)
{
	switch (n)
	{
		case 0:
			return TIM_Channel_1;
		case 1:
			return TIM_Channel_2;
		case 2:
			return TIM_Channel_3;
		case 3:
			return TIM_Channel_4;
		default:
			return TIM_Channel_1;	// error, this should not happen !!
	}
}


void tmr_set_timebase_update_cb(tmr_t *tmr, timebase_update_cb_t cb, int channel, void *param)
{
	tmr->timebase_update_cb_param[tmr_ch2n(channel)] = param;
	tmr->timebase_update_cb[tmr_ch2n(channel)] = cb;
}


void tmr_set_update_cb(tmr_t *tmr, tmr_update_cb_t cb, void *param)
{
	tmr->update_cb_param = param;
	tmr->update_cb = cb;
}


void tmr_set_timebase(tmr_t *tmr, uint32_t arr, uint16_t prescaler)
{
	uint8_t k;
	arr = CLIP(arr, 0, UINT16_MAX); ///@todo if this is a 32bit timer we can go higher

	// reconfigure the timer to set the desired period
	if (tmr_running(tmr))
	{
		// if the timer is already running we will switch to the new period
		// at the next update event to avoid glitching
		sys_enter_critical_section();
		TIM_SetAutoreload(tmr->tim, arr);
		TIM_PrescalerConfig(tmr->tim, prescaler, TIM_PSCReloadMode_Update);
		tmr->arr = arr;
		tmr->prescaler = prescaler;
		sys_leave_critical_section();
	}
	else
	{
		// the timer is not running so we can set this up fully via TimeBaseInit
		// since there is pretty much no penalty for this and it handles the init
		// case (this will basically handle some stuff that should be run in init 
		// (and wont matter if it run again) and use TIM_PSCReloadMode_Immediate
		TIM_TimeBaseStructInit(&tmr->cfg);
		tmr->cfg.TIM_Period = arr;
		tmr->cfg.TIM_Prescaler = prescaler;
		tmr->cfg.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(tmr->tim, &tmr->cfg);
		tmr->arr = arr;
		tmr->prescaler = prescaler;
	}

	// run this callback so other modules building on this know to
	// update their duty cycles etc
	for (k=0; k<4; k++)
		if (tmr->timebase_update_cb[k])
			tmr->timebase_update_cb[k](tmr, tmr_n2ch(k), tmr->timebase_update_cb_param[k]);

}


float tmr_set_period(tmr_t *tmr, float period)
{
	uint32_t tmr_freq = get_tmr_freq(tmr);
	uint32_t arr;
	uint32_t prescaler; ///@todo if this is a 32bit timer we can go higher
	uint32_t scale = lroundf((float)tmr_freq * period);

	// find the highest period value using the smallest pre-scaler (ie best
	// resolution), we could try to find the most precise frequency by looking
	// for factors of scale but this likely isn't as important as setting the
	// timer resolution in most applications
	for (prescaler = 1; prescaler <= UINT16_MAX + 1; prescaler++)
	{
		arr = scale / prescaler;
		if (arr <= UINT16_MAX + 1) ///@todo if this is a 32bit timer we can go higher
			goto done;
	}

	// if we got here we could not make a timer slow enough so we will do the
	// slowest we can !
done:
	tmr_set_timebase(tmr, MAX(arr - 1, 0), (uint16_t)CLIP(prescaler - 1, 0, UINT16_MAX));

	// return the actual period used
	return (float)(tmr->prescaler * tmr->arr) / (float)tmr_freq;
}


float tmr_set_freq(tmr_t *tmr, float freq)
{
	float res;

	// avoid div 0 errors by setting max period
	if (freq <= 0)
		res = tmr_set_period(tmr, FLT_MAX);
	else
		res = tmr_set_period(tmr, 1.0f/freq);

	return 1.0f/res;
}


uint32_t tmr_get_tick(tmr_t *tmr)
{
	return TIM_GetCounter(tmr->tim);
}


void tmr_init(tmr_t *tmr)
{
	NVIC_InitTypeDef nvic_init;

	// enable RCC for the tmr and setup the period and pre scaler
	init_rcc(tmr);

	// setup the tmr isr
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	nvic_init.NVIC_IRQChannel = tmr_irq(tmr);
	nvic_init.NVIC_IRQChannelPreemptionPriority = tmr->preemption_priority;
	nvic_init.NVIC_IRQChannelSubPriority = 0;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);

	// handle initial time sync setup
	if (tmr->sync.etr_pin)
		gpio_init_pin(tmr->sync.etr_pin);
	if (tmr->sync.edge_pin)
	{
		TIM_ICInitTypeDef ic_init =
		{
			.TIM_Channel = TIM_Channel_1,
			.TIM_ICPolarity = TIM_ICPolarity_Rising,
			.TIM_ICSelection = TIM_ICSelection_DirectTI,
			.TIM_ICPrescaler = TIM_ICPSC_DIV1,
			.TIM_ICFilter = 0,
		};
		gpio_init_pin(tmr->sync.edge_pin);
		TIM_ICInit(tmr->tim, &ic_init);
	}
	TIM_SelectMasterSlaveMode(tmr->tim, tmr->sync.master_slave);
	TIM_SelectOutputTrigger(tmr->tim, tmr->sync.output_trigger);
	TIM_SelectInputTrigger(tmr->tim, tmr->sync.input_trigger);
	tmr_sync_cfg(tmr, tmr->sync.ext_clk_mode, tmr->sync.slave_mode);

	// if a period is given use that, otherwise use a frequency if given
	// otherwise use the arr, prescaler values (if not give they default to 0,
	// ie go as fast as possible)
	if (tmr->period > 0.0f)
		tmr_set_period(tmr, tmr->period);
	else if (tmr->freq > 0.0f)
		tmr_set_freq(tmr, tmr->freq);
	else
		tmr_set_timebase(tmr, tmr->arr, tmr->prescaler);
	dbg_stop(tmr);
}


static void tmr_irq_handler(int n)
{
	tmr_t *tmr = tmr_irq_list[n];
	tmr_update_cb_t update_cb = NULL;
	void *update_cb_param = NULL;


	if (tmr == NULL)
		return;

	if (TIM_GetITStatus(tmr->tim, TIM_IT_Update))
	{
		TIM_ClearITPendingBit(tmr->tim, TIM_IT_Update);
		if (tmr->update_cb != NULL)
		{
			update_cb = tmr->update_cb;
			update_cb_param = tmr->update_cb_param;
		}
	}

	// run differed callbacks
	if (update_cb)
		update_cb(tmr, update_cb_param);
}


void TIM2_IRQHandler(void)
{
	tmr_irq_handler(2);
}


void TIM3_IRQHandler(void)
{
	tmr_irq_handler(3);
}


void TIM4_IRQHandler(void)
{
	tmr_irq_handler(4);
}


void TIM5_IRQHandler(void)
{
	tmr_irq_handler(5);
}


///@todo break this out into common code for dma is unerrun/overrun events are needed !!
void TIM6_DAC1_IRQHandler(void)
{
	tmr_irq_handler(6);
}


void TIM7_IRQHandler(void)
{
	tmr_irq_handler(7);
}


void TIM12_IRQHandler(void)
{
	tmr_irq_handler(12);
}


void TIM13_IRQHandler(void)
{
	tmr_irq_handler(13);
}


void TIM14_IRQHandler(void)
{
	tmr_irq_handler(14);
}


///@todo break this out into common code for dma is unerrun/overrun events are needed !!
void TIM18_DAC2_IRQHandler(void)
{
	tmr_irq_handler(18);
}

