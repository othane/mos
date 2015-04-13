
/**
 * @file ppm_hw.h
 *
 * @brief this contains hw definitions for configuration via hw.c only (it is not a run time interface !)
 *
 * @author OT
 *
 * @date Jan 2014
 *
 */

#ifndef __PPM_HW__
#define __PPM_HW__


#include "hal.h"
#include "tmr_hw.h"
#include "gpio_hw.h"


// internal representation of a ppm channel
typedef struct ppm_channel_t ppm_channel_t;
struct ppm_channel_t
{
	tmr_t *tmr;						///< link to the parent timer
	gpio_pin_t *pin;				///< output pin or NULL for none

	uint16_t ch;					///< timer channel (can be TIM_Channel_1 .. TIM_Channel_4)
	float phs;						///< phase of the PPM from 0..1.0 (pi rads)
	uint32_t freq;					///< frequency of the ppm signal
	TIM_OCInitTypeDef oc_cfg;		///< TIM_OCMode must be TIM_OCMode_Toggle
};

#endif

