/**
 * @file hw.c
 * 
 * @brief implements the default stm32f107x hw available
 *
 * @see hw.h for instructions to override the defaults
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */

#include <hal.h>


#if defined STM32F10X_CL
	#include <stm32f10x_conf.h>
	#error "adc not supported on stm32f107x"

#elif defined STM32F37X

	#include <stm32f37x_conf.h>
	#include <gpio_hw.h>
	#include <adc_hw.h>

	gpio_pin_t adc_pe12_pin = 
	{
		.port = GPIOE,
		.cfg = {.GPIO_Pin = GPIO_Pin_12, .GPIO_Mode = GPIO_Mode_AN, 
				.GPIO_PuPd = GPIO_PuPd_NOPULL},
	};


	gpio_pin_t adc_pe11_pin = 
	{
		.port = GPIOE,
		.cfg = {.GPIO_Pin = GPIO_Pin_11, .GPIO_Mode = GPIO_Mode_AN, 
				.GPIO_PuPd = GPIO_PuPd_NOPULL},
	};


	gpio_pin_t adc_pe10_pin = 
	{
		.port = GPIOE,
		.cfg = {.GPIO_Pin = GPIO_Pin_10, .GPIO_Mode = GPIO_Mode_AN, 
				.GPIO_PuPd = GPIO_PuPd_NOPULL},
	};


	gpio_pin_t adc_pe9_pin = 
	{
		.port = GPIOE,
		.cfg = {.GPIO_Pin = GPIO_Pin_9, .GPIO_Mode = GPIO_Mode_AN, 
				.GPIO_PuPd = GPIO_PuPd_NOPULL},
	};


	dma_t adc1_dma =
	{
		.channel = DMA2_Channel3,
	};


	adc_t adc1 =
	{
		.base = SDADC1,
		.ref = SDADC_VREF_Ext,
		.SDADC_AINStructure = 
		{
			{.SDADC_InputMode = SDADC_InputMode_SEZeroReference, .SDADC_Gain = SDADC_Gain_1,
				.SDADC_CommonMode = SDADC_CommonMode_VSSA},
			{.SDADC_InputMode = SDADC_InputMode_SEZeroReference, .SDADC_Gain = SDADC_Gain_1,
				.SDADC_CommonMode = SDADC_CommonMode_VSSA},
			{.SDADC_InputMode = SDADC_InputMode_SEZeroReference, .SDADC_Gain = SDADC_Gain_1,
				.SDADC_CommonMode = SDADC_CommonMode_VSSA},
		},
		.dma = &adc1_dma,
	};


	adc_channel_t adc_chanA =
	{
		.adc = &adc1,
		.number = SDADC_Channel_0,
		.conf = SDADC_Conf_0,
		.pin = &adc_pe12_pin,
	};


	adc_channel_t adc_chanB =
	{
		.adc = &adc1,
		.number = SDADC_Channel_1,
		.conf = SDADC_Conf_0,
		.pin = &adc_pe11_pin,
	};


	adc_channel_t adc_chanC =
	{
		.adc = &adc1,
		.number = SDADC_Channel_2,
		.conf = SDADC_Conf_0,
		.pin = &adc_pe10_pin,
	};


	adc_channel_t adc_chanD =
	{
		.adc = &adc1,
		.number = SDADC_Channel_7,
		.conf = SDADC_Conf_0,
		.pin = &adc_pe9_pin,
	};

	adc_channel_t *adc_chan = &adc_chanC;

#elif defined STM32F40_41xxx

	#include <stm32f4xx_conf.h>
	#include <gpio_hw.h>
	#include <adc_hw.h>

	gpio_pin_t adc_pa0_pin = 
	{
		.port = GPIOA,
		.cfg = {.GPIO_Pin = GPIO_Pin_0, .GPIO_Mode = GPIO_Mode_AN, 
				.GPIO_PuPd = GPIO_PuPd_NOPULL},
	};


	gpio_pin_t adc_pa1_pin = 
	{
		.port = GPIOA,
		.cfg = {.GPIO_Pin = GPIO_Pin_1, .GPIO_Mode = GPIO_Mode_AN, 
				.GPIO_PuPd = GPIO_PuPd_NOPULL},
	};


	gpio_pin_t adc_pa2_pin = 
	{
		.port = GPIOA,
		.cfg = {.GPIO_Pin = GPIO_Pin_2, .GPIO_Mode = GPIO_Mode_AN, 
				.GPIO_PuPd = GPIO_PuPd_NOPULL},
	};


	dma_t adc1_dma =
	{
		.stream = DMA2_Stream0,
		.channel = DMA_Channel_0,
	};


	adc_t adc1 =
	{
		.base = ADC1,
		.trigger = {
			.type = ADC_ExternalTrigConvEdge_None,
		},
		.dma = &adc1_dma,
	};


	adc_channel_t adc_chan0 =
	{
		.adc = &adc1,
		.number = ADC_Channel_0,
		.sample_time = ADC_SampleTime_15Cycles,
		.pin = &adc_pa0_pin,
	};


	adc_channel_t adc_chan1 =
	{
		.adc = &adc1,
		.number = ADC_Channel_1,
		.sample_time = ADC_SampleTime_15Cycles,
		.pin = &adc_pa1_pin,
	};


	adc_channel_t adc_chan2 =
	{
		.adc = &adc1,
		.number = ADC_Channel_2,
		.sample_time = ADC_SampleTime_15Cycles,
		.pin = &adc_pa2_pin,
	};

	adc_channel_t *adc_chan = &adc_chan0;

#else

	#error "adc not supported on unknown target"

#endif
