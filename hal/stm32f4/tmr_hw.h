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

	float period;	// initial period of the tmr (set either period xor freq)
	float freq;		// "
	uint32_t arr;
	uint16_t prescaler;
	TIM_TimeBaseInitTypeDef cfg;
	uint8_t stop_on_halt;
	uint8_t preemption_priority;

	struct
	{
		uint16_t master_slave; 		// see TIM_Master_Slave_Mode (sync timers)
		uint16_t slave_mode;		// see TIM_Slave_Mode or 0 if not slave
		uint16_t output_trigger;	// see TIM_Trigger_Output_Source
		uint16_t input_trigger;		// see TIM_Internal_Trigger_Selection (table 45 in reference manual)
		gpio_pin_t *edge_pin;		// optional gpio for edge detector TI1F_ED
		uint8_t ext_clk_mode;		// {0: internal, 1: external mode 1, 2: external mode 2}
		uint32_t ext_clk_freq;		// if ext_clk_mode is !0 this is the assumed clk frequency for set freq/period
		gpio_pin_t *etr_pin;		// optional gpio for the ETR pin
	} sync;

	timebase_update_cb_t timebase_update_cb[4];
	void *timebase_update_cb_param[4];

	tmr_update_cb_t update_cb;
	void *update_cb_param;
};

#endif

