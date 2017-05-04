/**
 * @file ppm.c
 *
 * @brief implement the ppm module for the stm32f373
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
#include "ppm_hw.h"


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


void ppm_start(ppm_channel_t *ppm)
{
	tmr_start(ppm->tmr);
}


void ppm_stop(ppm_channel_t *ppm)
{
	tmr_stop(ppm->tmr);
}


void ppm_set_phs(ppm_channel_t *ppm, float phs)
{
	struct tmr_t *tmr = ppm->tmr;
	uint32_t ccr;

	// turn phs into a ccr value
	if (phs < 0.5)
	{
		ccr = ppm->tmr->arr * 2 * phs - 1;
		ppm->oc_cfg.TIM_OCPolarity = TIM_OCPolarity_High;
	}
	else
	{
		ccr = ppm->tmr->arr * 2 * (phs - 0.5) - 1;
		ppm->oc_cfg.TIM_OCPolarity = TIM_OCPolarity_Low;
	}
	
	if (!tmr_running(tmr))
	{
		ppm->oc_cfg.TIM_Pulse = ccr;
		ppm->oc_cfg.TIM_OCMode = TIM_OCMode_Toggle; // magic to make the ppm work, but duty has to be 50%
		TIM_OCxInit(tmr->tim, &ppm->oc_cfg, ppm->ch);
	}
	else
	{
		// else we are running so update the CCR from phs asap (this will take
		// effect after the next timer update event
		sys_enter_critical_section();
		switch (ppm->ch)
		{
			case TIM_Channel_1:
				TIM_SetCompare1(tmr->tim, ccr);
				TIM_OC1PolarityConfig(tmr->tim, ppm->oc_cfg.TIM_OCPolarity);
				break;
			case TIM_Channel_2:
				TIM_SetCompare2(tmr->tim, ccr);
				TIM_OC2PolarityConfig(tmr->tim, ppm->oc_cfg.TIM_OCPolarity);
				break;
			case TIM_Channel_3:
				TIM_SetCompare3(tmr->tim, ccr);
				TIM_OC3PolarityConfig(tmr->tim, ppm->oc_cfg.TIM_OCPolarity);
				break;
			case TIM_Channel_4:
				TIM_SetCompare4(tmr->tim, ccr);
				TIM_OC4PolarityConfig(tmr->tim, ppm->oc_cfg.TIM_OCPolarity);
				break;
		}
		sys_leave_critical_section();
	}
}


uint32_t ppm_set_freq(ppm_channel_t *ppm, uint32_t freq)
{
	// for toggle to work for ppm we need the timer run twice as fast
	// see https://my.st.com/public/STe2ecommunities/mcu/Lists/cortex_mx_stm32/Flat.aspx?RootFolder=https%3a%2f%2fmy.st.com%2fpublic%2fSTe2ecommunities%2fmcu%2fLists%2fcortex_mx_stm32%2fPWM%20same%20Dutycycle%2c%20different%20starttime%20%28Phaseshift%29&FolderCTID=0x01200200770978C69A1141439FE559EB459D7580009C4E14902C3CDE46A77F0FFD06506F5B&currentviews=1594
	freq = tmr_set_freq(ppm->tmr, 2*freq);
	// note the phase will be updated from the ppm_update_phs_on_freq_change below by the tmr
	return freq / 2;
}

static void ppm_update_phs_on_freq_change(tmr_t *tmr, int ch, void *param)
{
	ppm_channel_t *ppm = (ppm_channel_t *)param;

	// update this channels phase for the new timer freq
	ppm_set_phs(ppm, ppm->phs);
}

void ppm_init(ppm_channel_t *ppm)
{
	// init the gpio in AF mode
	gpio_init_pin(ppm->pin);

	// init ppm parent timer module (sets freq and phs via ppm_update_phs_on_freq_change cb)
	tmr_set_timebase_update_cb(ppm->tmr, ppm_update_phs_on_freq_change, ppm->ch, ppm);
	ppm->tmr->freq = ppm->freq * 2; // double frequency since we use toggle magic to make this work which needs freq*2 to the timer
	tmr_init(ppm->tmr);
}


