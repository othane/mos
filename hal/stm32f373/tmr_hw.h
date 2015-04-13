/**
 * @file tmr_hw.h
 *
 * @brief this contains hw definitions for configuration via hw.c only (it is not a run time interface !)
 *
 * @author OT
 *
 * @date Jan 2014
 *
 */

#ifndef __TMR_HW__
#define __TMR_HW__


#include "hal.h"


// internal representation of a tmr
typedef struct tmr_t tmr_t;
struct tmr_t
{
	TIM_TypeDef *tim;				///< which timer do you want to use

	uint32_t freq;
	uint32_t period;
	TIM_TimeBaseInitTypeDef cfg;
};

#endif

