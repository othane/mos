/**
 * @file tmr.c
 *
 * @brief implement the timer module for the stm32f4
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


#include <stm32f4xx_conf.h>
#include <math.h>
#include <float.h>
#include "hal.h"
#include "tmr_hw.h"

#define MIN(a,b) ((a < b)? a: b)
#define MAX(a,b) ((a > b)? a: b)
#define CLIP(a, amin, amax) MAX((MIN(a, amax)), amin)


// look up the irq channel for this tmr
static tmr_t *tmr_irq_list[14] = {NULL,};  ///< just store the tmr handle so we can get it in the irq (then hw.c is more free form)
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
		case (uint32_t)TIM7:
			tmr_irq_list[7] = tmr;
			return TIM7_IRQn;
		default:
			///@todo error
			return 0x00;
	}
}

static void init_rcc(struct tmr_t *tmr)
{
	switch ((int)tmr->tim)
	{
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
		case (int)TIM7:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
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
		case (int)TIM7:
			DBGMCU_APB1PeriphConfig(DBGMCU_TIM7_STOP, state);
			break;
		default:
			///@todo warn timer is not valid
			break;
	}
}


static uint32_t get_tmr_freq(struct tmr_t *tmr, uint32_t sys_freq)
{
	switch ((int)tmr->tim)
	{
		// these timers are on the APB1 bus which is 1/4 the speed of the
		// sys_clk but for the timers the APB1 clk is already x2 because the
		// APB1 pre-scaler is already /4 (see ref manual p217)
		case (int)TIM2:
		case (int)TIM3:
		case (int)TIM4:
		case (int)TIM5:
		case (int)TIM7:
		default:
			return (sys_freq >> 1);
	}
}


static void tmr_sync_cfg(struct tmr_t *tmr)
{
	uint16_t slave_mode = tmr->sync.slave_mode;

	TIM_SelectMasterSlaveMode(tmr->tim, tmr->sync.master_slave);
	TIM_SelectSlaveMode(tmr->tim, slave_mode);
	TIM_SelectOutputTrigger(tmr->tim, tmr->sync.output_trigger);
	TIM_SelectInputTrigger(tmr->tim, tmr->sync.input_trigger);
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


float tmr_set_period(tmr_t *tmr, float period)
{
	uint32_t sys_freq = sys_clk_freq();
	uint32_t tmr_freq = get_tmr_freq(tmr, sys_freq);
	uint32_t arr;
	uint32_t prescaler; ///@todo if this is a 32bit timer we can go higher
	uint32_t scale = lroundf((float)tmr_freq * period);
	uint8_t k;

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
	arr = CLIP(arr, 1, UINT16_MAX); ///@todo if this is a 32bit timer we can go higher

	// reconfigure the timer to set the desired period
	if (tmr_running(tmr))
	{
		// if the timer is already running we will switch to the new period
		// at the next update event to avoid glitching
		sys_enter_critical_section();
		TIM_SetAutoreload(tmr->tim, arr - 1);
		tmr->arr = arr;
		TIM_PrescalerConfig(tmr->tim, prescaler - 1, TIM_PSCReloadMode_Update);
		sys_leave_critical_section();
	}
	else
	{
		// the timer is not running so we can set this up fully via TimeBaseInit
		// since there is pretty much no penalty for this and it handles the init
		// case (this will basically handle some stuff that should be run in init 
		// (and wont matter if it run again) and use TIM_PSCReloadMode_Immediate
		TIM_TimeBaseStructInit(&tmr->cfg);
		tmr->cfg.TIM_Period = arr - 1;
		tmr->cfg.TIM_Prescaler = prescaler - 1;
		tmr->cfg.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(tmr->tim, &tmr->cfg);
		tmr->arr = arr;
	}

	// run this callback so other modules building on this know to
	// update their duty cycles etc
	for (k=0; k<4; k++)
		if (tmr->timebase_update_cb[k])
			tmr->timebase_update_cb[k](tmr, tmr_n2ch(k), tmr->timebase_update_cb_param[k]);

	// return the actual period used
	return (float)(prescaler * arr) / (float)tmr_freq;
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

	// set either default period or frequency depending what is given
	// if none we set to the slowest timer possible (smallest freq)
	if (tmr->period > 0.0f)
		tmr_set_period(tmr, tmr->period);
	else
		tmr_set_freq(tmr, tmr->freq);
	tmr_sync_cfg(tmr);
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

void TIM7_IRQHandler(void)
{
	tmr_irq_handler(7);
}

