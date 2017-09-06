/**
 * @file pwm_hw.h
 *
 * @brief this contains hw definitions for configuration via hw.c only (it is not a run time interface !)
 *
 * @author OT
 *
 * @date Jan 2014
 *
 */

#ifndef __PWM_HW__
#define __PWM_HW__


#include "hal.h"
#include "tmr_hw.h"
#include "gpio_hw.h"


// internal representation of a pwm channel
typedef struct pwm_channel_t pwm_channel_t;
struct pwm_channel_t
{
	tmr_t *tmr;						///< link to the parent timer
	gpio_pin_t *pin;				///< output pin or NULL for none

	uint16_t ch;					///< timer channel (can be TIM_Channel_1 .. TIM_Channel_4)
	float duty;						///< duty of the PWM from 0..1.0 (100%)
	TIM_OCInitTypeDef oc_cfg;		///< configure mode as either TIM_OCMode_PWM1|2
};

#endif

