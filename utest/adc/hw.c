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

gpio_pin_t adc_chanA_pin = 
{
	.port = GPIOE,
	.cfg = {.GPIO_Pin = GPIO_Pin_12, .GPIO_Mode = GPIO_Mode_AN, 
			.GPIO_Speed = GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL},
};


gpio_pin_t adc_chanB_pin = 
{
	.port = GPIOE,
	.cfg = {.GPIO_Pin = GPIO_Pin_11, .GPIO_Mode = GPIO_Mode_AN, 
			.GPIO_Speed = GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL},
};


gpio_pin_t adc_chanC_pin = 
{
	.port = GPIOE,
	.cfg = {.GPIO_Pin = GPIO_Pin_10, .GPIO_Mode = GPIO_Mode_AN, 
			.GPIO_Speed = GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL},
};


gpio_pin_t adc_chanD_pin = 
{
	.port = GPIOE,
	.cfg = {.GPIO_Pin = GPIO_Pin_9, .GPIO_Mode = GPIO_Mode_AN, 
			.GPIO_Speed = GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL},
};


adc_t adc0 =
{
	.base = SDADC1,
	.ref = SDADC_VREF_Ext,
	.SDADC_AINStructure = 
	{
		{.SDADC_InputMode = SDADC_InputMode_SEZeroReference, .SDADC_Gain = SDADC_Gain_1,
			.SDADC_CommonMode = SDADC_CommonMode_VSSA, .SDADC_Offset = 0},
		{.SDADC_InputMode = SDADC_InputMode_SEZeroReference, .SDADC_Gain = SDADC_Gain_1,
			.SDADC_CommonMode = SDADC_CommonMode_VSSA, .SDADC_Offset = 0},
		{.SDADC_InputMode = SDADC_InputMode_SEZeroReference, .SDADC_Gain = SDADC_Gain_1,
			.SDADC_CommonMode = SDADC_CommonMode_VSSA, .SDADC_Offset = 0},
	},
};


adc_channel_t adc_chanA =
{
	.adc = &adc0,
	.number = SDADC_Channel_0,
	.conf = SDADC_Conf_0,
	.pin = &adc_chanA_pin,
};


adc_channel_t adc_chanB =
{
	.adc = &adc0,
	.number = SDADC_Channel_1,
	.conf = SDADC_Conf_0,
	.pin = &adc_chanB_pin,
};


adc_channel_t adc_chanC =
{
	.adc = &adc0,
	.number = SDADC_Channel_2,
	.conf = SDADC_Conf_0,
	.pin = &adc_chanC_pin,
};


adc_channel_t adc_chanD =
{
	.adc = &adc0,
	.number = SDADC_Channel_7,
	.conf = SDADC_Conf_0,
	.pin = &adc_chanD_pin,
};


#endif
