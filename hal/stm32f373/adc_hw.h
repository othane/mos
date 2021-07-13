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
	void *base;	///< which adc to use (either SDADC_TypeDef or ADC_TypeDef)

	// common config
	dma_t *dma;				///< link to dma, trace requires dma for this module
	struct
	{
		uint32_t source;	///< trigger source see @ref SDADC_ExternalTrigger_sources or  @ref ADC_external_trigger_sources_for_regular_channels_conversion
		uint32_t cont;		///< 0 to trigger on first event and let it run, otherwise this requires a trigger for each sample [ie count triggers] (for some reason this only seems to work up till about 12KHz, not 16.66-50KHz as expected)
		uint8_t sync_adc1;	///< [sdadc only] start this adc with adc 1 start (different from below)
		uint32_t type;		///< [sdadc only] trigger type see SDADC_external_trigger_edge_for_injected_channels_conversion 
	} trigger;

	bool initalised;
	enum adc_type {
		SDADC=0,
		ADC,
	} type;
	dma_request_t dma_req;
	uint32_t adc_clk_div;

	// sdadc config only
	uint32_t ref;
	SDADC_AINStructTypeDef SDADC_AINStructure[3];
};


// internal representation of a adc channel
struct adc_channel_t
{
	adc_t *adc;				///< parent adc
	uint32_t number;		///< which channel is this adc @ref SDADC_Channel or @ref ADC_Channel
	uint8_t sample_time;	///< [no effect for sdadc] sample rate for this channel @ref ADC_sampling_times
	uint32_t conf;			///< [sdadc only] configuration to use
	gpio_pin_t *pin;		///< input pin
	gpio_pin_t *pin_ref;	///< [sdadc only] in differential mode use this pin as the - and pin as the +, else NULL

	adc_trace_complete_t complete;
	void *complete_param;
	volatile int16_t *buf;
	int count;
};


#endif


