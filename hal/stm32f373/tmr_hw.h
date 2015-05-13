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

	struct
	{
		uint16_t master_slave; 		// see TIM_Master_Slave_Mode (sync timers)
		uint16_t slave_mode;		// see TIM_Slave_Mode or 0 if not slave
		uint16_t output_trigger;	// see TIM_Trigger_Output_Source
		uint16_t input_trigger;		// see TIM_Internal_Trigger_Selection (table 45 in reference manual)
	} sync;
};

#endif

