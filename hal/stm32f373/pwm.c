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


static void TIM_OCxInit(TIM_TypeDef *tim, TIM_OCInitTypeDef *oc_cfg, uint16_t ch)
{
	switch (ch)
	{
		case TIM_Channel_1:
			TIM_OC1Init(tim, oc_cfg);
			TIM_OC1PreloadConfig(tim, TIM_OCPreload_Enable); // just do this to avoid glitching when changing the duty
			break;
		case TIM_Channel_2:
			TIM_OC2Init(tim, oc_cfg);
			TIM_OC2PreloadConfig(tim, TIM_OCPreload_Enable);
			break;
		case TIM_Channel_3:
			TIM_OC3Init(tim, oc_cfg);
			TIM_OC3PreloadConfig(tim, TIM_OCPreload_Enable);
			break;
		case TIM_Channel_4:
			TIM_OC4Init(tim, oc_cfg);
			TIM_OC4PreloadConfig(tim, TIM_OCPreload_Enable);
			break;
	}
}


void pwm_start(pwm_channel_t *pwm)
{
	tmr_start(pwm->tmr);
}


void pwm_stop(pwm_channel_t *pwm)
{
	tmr_stop(pwm->tmr);
}


void pwm_reset(pwm_channel_t *pwm)
{
	tmr_reset(pwm->tmr);
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
			ccr = round(pwm->tmr->arr * duty);
			break;
		case TIM_OCMode_PWM2:
			ccr = round(pwm->tmr->arr * (1 - duty));
			break;
		default:
			///@todo error invalid mode
			break;
	}

	if (!tmr_running(tmr))
	{
		// if the timer is stopped then we will re-init the whole thing (there is no real
		// down side in doing this and it handles the init case easily)
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


uint32_t pwm_set_freq(pwm_channel_t *pwm, uint32_t freq)
{
	freq = tmr_set_freq(pwm->tmr, freq);
	// note the duty will be updated from the pwm_update_duty_on_freq_change below by the tmr
	return freq;
}


static void pwm_update_duty_on_freq_change(tmr_t *tmr, int ch, void *param)
{
	pwm_channel_t *pwm = (pwm_channel_t *)param;

	// update this channels duty for the new timer freq
	pwm_set_duty(pwm, pwm->duty);
}


void pwm_init(pwm_channel_t *pwm)
{
	// init the gpio in AF mode
	gpio_init_pin(pwm->pin);

	// init pwm parent timer module (sets freq and duty via pwm_update_duty_on_freq_change cb)
	tmr_set_timebase_update_cb(pwm->tmr, pwm_update_duty_on_freq_change, pwm->ch, pwm);
	tmr_init(pwm->tmr);
}

