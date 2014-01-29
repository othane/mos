/**
 * @file adc.h
 *
 * @brief this contains hw definitions for configuration via hw.c only (it is not a run time interface !)
 *
 * @author OT
 *
 * @date Jan 2014
 *
 */

#ifndef __ADC_HW__
#define __ADC_HW__


#include "hal.h"
#include "gpio_hw.h"


// internal representation of a adc
typedef struct adc_t adc_t;
struct adc_t
{
	SDADC_TypeDef *base;
	uint32_t ref;
	SDADC_AINStructTypeDef SDADC_AINStructure[3];
	bool initalised;
};


// internal representation of a adc channel
struct adc_channel_t
{
	adc_t *adc;
	uint32_t number;
	uint32_t conf;
	gpio_pin_t *pin;
};


#endif


