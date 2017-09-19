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
	ADC_TypeDef *base;		///< which adc to use

	// stm config
	struct
	{
		uint32_t type;		///< trigger type @ref ADC_external_trigger_edge_for_regular_channels_conversion
		uint32_t source;	///< trigger source @ref ADC_extrenal_trigger_sources_for_regular_channels_conversion
		uint32_t cont;		///< 0 to trigger on first event and let it run, otherwise this requires a trigger for each sample [ie count triggers] (for some reason this only seems to work up till about 12KHz, not 16.66-50KHz as expected)
	} trigger;

	dma_t *dma;				///< link to dma, trace requires dma for this module

	bool initalised;
	dma_request_t dma_req;
};


// internal representation of a adc channel
struct adc_channel_t
{
	adc_t *adc;				///< parent adc
	uint8_t number;			///< which channel is this adc @ref ADC_Channel
	uint8_t sample_time;	///< sample rate for this channel @ref ADC_sampling_times 
	gpio_pin_t *pin;		///< input pin

	adc_trace_complete_t complete;
	void *complete_param;
	volatile int16_t *buf;
	int count;
};


#endif


