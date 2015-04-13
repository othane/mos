/**
 * @file pwm.c
 *
 * @brief implement the pwm module for the stm32f373
 *
 * @author OT
 *
 * @date Apr 2015
 *
 */


#include <stm32f37x_conf.h>
#include <math.h>
#include "hal.h"
#include "gpio_hw.h"
#include "pwm_hw.h"


#define TMR_RUNNING(tmr) (tmr->tim->CR1 & TIM_CR1_CEN? 1: 0)


static void TIM_OCxInit(TIM_TypeDef *tim, TIM_OCInitTypeDef *oc_cfg, uint16_t ch)
{
	switch (ch)
	{
		case TIM_Channel_1:
			TIM_OC1Init(tim, oc_cfg);
			break;
		case TIM_Channel_2:
			TIM_OC2Init(tim, oc_cfg);
			break;
		case TIM_Channel_3:
			TIM_OC3Init(tim, oc_cfg);
			break;
		case TIM_Channel_4:
			TIM_OC4Init(tim, oc_cfg);
			break;
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


void pwm_start(pwm_channel_t *pwm)
{
	TIM_Cmd(pwm->tmr->tim, ENABLE);
}


void pwm_stop(pwm_channel_t *pwm)
{
	TIM_Cmd(pwm->tmr->tim, DISABLE);
}


void pwm_set_duty(pwm_channel_t *pwm, float duty)
{
	struct tmr_t *tmr = pwm->tmr;
	uint32_t ccr;

	// calc ccr value from duty (duty is always high time so this is slightly
	// different for PWM mode 1 & 2)
	switch (pwm->oc_cfg.TIM_OCMode)
	{
		case TIM_OCMode_PWM1:
			ccr = round(pwm->tmr->period * duty);
			break;
		case TIM_OCMode_PWM2:
			ccr = round(pwm->tmr->period * (1 - duty));
			break;
		default:
			///@todo error invalid mode
			break;
	}

	if (!TMR_RUNNING(tmr))
	{
		// if the timer is stopped then we will re-init the whole thing (there is no real
		// down side in doing this and it handles the init case easily)
		TIM_CCPreloadControl(tmr->tim, ENABLE); // just do this to avoid glitching when changing the duty
		pwm->oc_cfg.TIM_Pulse = ccr;
		TIM_OCxInit(tmr->tim, &pwm->oc_cfg, pwm->ch);
		tmr->tim->EGR = TIM_PSCReloadMode_Immediate;
	}
	else
	{
		// else we are running so update the CCR from duty asap (this will take
		// effect after the next timer update event
		sys_enter_critical_section();
		switch (pwm->ch)
		{
			case TIM_Channel_1:
				TIM_SetCompare1(tmr->tim, ccr);
				break;
			case TIM_Channel_2:
				TIM_SetCompare2(tmr->tim, ccr);
				break;
			case TIM_Channel_3:
				TIM_SetCompare3(tmr->tim, ccr);
				break;
			case TIM_Channel_4:
				TIM_SetCompare4(tmr->tim, ccr);
				break;
		}
		sys_leave_critical_section();
	}
}


int pwm_set_freq(pwm_channel_t *pwm, uint32_t freq)
{
	uint32_t tmr_freq = sys_clk_freq();
	uint32_t period, prescaler;
	struct tmr_t *tmr = pwm->tmr;

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
	if (TMR_RUNNING(tmr))
	{
		// if the timer is already running we will switch to the new frequency
		// at the next update event to avoid glitching
		sys_enter_critical_section();
		TIM_SetAutoreload(tmr->tim, period - 1);
		pwm->tmr->period = period;
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
		pwm->tmr->period = period;
	}
	pwm_set_duty(pwm, pwm->duty);

	// return the actual frequency used
	return tmr_freq / (period * prescaler);
}


void pwm_init(pwm_channel_t *pwm)
{
	// enable RCC for the TIM
	init_rcc(pwm->tmr);
	
	// init the gpio in AF mode
	gpio_init_pin(pwm->pin);
	
	// init the timer and pwm
	pwm_set_freq(pwm, pwm->freq);
}

