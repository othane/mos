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
#include "hal.h"
#include "tmr_hw.h"

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


void tmr_start(tmr_t *tmr)
{
	TIM_Cmd(tmr->tim, ENABLE);
}


void tmr_stop(tmr_t *tmr)
{
	TIM_Cmd(tmr->tim, DISABLE);
}


int tmr_running(tmr_t *tmr)
{
	return (tmr->tim->CR1 & TIM_CR1_CEN? 1: 0);
}


uint32_t tmr_set_freq(tmr_t *tmr, uint32_t freq)
{
	uint32_t tmr_freq = sys_clk_freq();
	uint32_t period, prescaler;

	// find the highest period value using the smallest pre-scaler (ie best resolution)
	for (prescaler = 1; prescaler <= UINT16_MAX; prescaler++)
	{
		period = tmr_freq / (freq * prescaler);
		if (period <= UINT16_MAX)
			goto done;
	}

	// if we got here we could not make a PWM slow enough so we will do the
	// slowest we can !

done:
	// reconfigure the timer to set the desired frequency
	if (tmr_running(tmr))
	{
		// if the timer is already running we will switch to the new frequency
		// at the next update event to avoid glitching
		sys_enter_critical_section();
		TIM_SetAutoreload(tmr->tim, period - 1);
		tmr->period = period;
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
		tmr->cfg.TIM_Period = period - 1;
		tmr->cfg.TIM_Prescaler = prescaler - 1;
		tmr->cfg.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(tmr->tim, &tmr->cfg);
		tmr->period = period;
	}

	// return the actual frequency used
	return tmr_freq / (period * prescaler);
}


uint32_t tmr_get_tick(tmr_t *tmr)
{
	return TIM_GetCounter(tmr->tim);
}


void tmr_init(tmr_t *tmr)
{
	// enable RCC for the tmr and setup the period and pre scaler
	init_rcc(tmr);
	tmr_set_freq(tmr, tmr->freq);
}

