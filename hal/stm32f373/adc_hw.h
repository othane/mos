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
#include "dma_hw.h"


// internal representation of a adc
typedef struct adc_t adc_t;
struct adc_t
{
	SDADC_TypeDef *base;	///< which adc to use
	uint32_t ref;
	SDADC_AINStructTypeDef SDADC_AINStructure[3];
	uint32_t sadc_clk_div;
	dma_t *dma;				///< link to dma, trace requires dma for this module
	struct
	{
		uint32_t type;		///< trigger type see SDADC_external_trigger_edge_for_injected_channels_conversion 
		uint32_t source;	///< trigger source see SDADC_ExternalTrigger_sources
		uint32_t cont;		///< 0 to trigger on first event and let it run, otherwise this requires a trigger for each sample (ie count triggers)
	} trigger;

	bool initalised;
	dma_request_t dma_req;
};


// internal representation of a adc channel
struct adc_channel_t
{
	adc_t *adc;
	uint32_t number;
	uint32_t conf;
	gpio_pin_t *pin;
	
	adc_trace_complete_t complete;
	void *complete_param;
	volatile int16_t *buf;
	int count;
};


#endif


