/**
 * @file adc.c
 *
 * @brief implement the adc module for the stm32f373
 *
 * @author OT
 *
 * @date Jan 2014
 *
 */


#include <stm32f37x_conf.h>
#include "hal.h"
#include "gpio_hw.h"
#include "adc_hw.h"


static uint32_t adc_apb2_periph(SDADC_TypeDef *base)
{
	switch ((uint32_t)base)
	{
		case (uint32_t)SDADC1:
			return RCC_APB2Periph_SDADC1;
		case (uint32_t)SDADC2:
			return RCC_APB2Periph_SDADC2;
		case (uint32_t)SDADC3:
			return RCC_APB2Periph_SDADC3;
		default:
			return RCC_APB2Periph_SDADC1;
	}
}


static uint32_t adc_pwr_periph(SDADC_TypeDef *base)
{
	switch ((uint32_t)base)
	{
		case (uint32_t)SDADC1:
			return PWR_SDADCAnalog_1;
		case (uint32_t)SDADC2:
			return PWR_SDADCAnalog_2;
		case (uint32_t)SDADC3:
			return PWR_SDADCAnalog_3;
		default:
			return PWR_SDADCAnalog_1;
	}
}


static void adc_init(adc_t *adc)
{
	// set adc clk to 6MHz
	RCC_APB2PeriphClockCmd(adc_apb2_periph(adc->base), ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_SDADCAnalogCmd(adc_pwr_periph(adc->base), ENABLE);
	RCC_SDADCCLKConfig(RCC_SDADCCLK_SYSCLK_Div12);

	// setup the different adc configurations
	SDADC_VREFSelect(adc->ref);
	sys_spin(5);
	SDADC_Cmd(adc->base, ENABLE);
	SDADC_InitModeCmd(adc->base, ENABLE);
	while (SDADC_GetFlagStatus(adc->base, SDADC_FLAG_INITRDY) == RESET)
	{}
	SDADC_AINInit(adc->base, SDADC_Conf_0, &adc->SDADC_AINStructure[0]);
	SDADC_AINInit(adc->base, SDADC_Conf_1, &adc->SDADC_AINStructure[1]);
	SDADC_AINInit(adc->base, SDADC_Conf_2, &adc->SDADC_AINStructure[2]);
	SDADC_InitModeCmd(adc->base, DISABLE);

	// calibrate
	SDADC_CalibrationSequenceConfig(adc->base, SDADC_CalibrationSequence_3);
	SDADC_StartCalibration(adc->base);
	while (SDADC_GetFlagStatus(adc->base, SDADC_FLAG_EOCAL) == RESET)
	{}

	adc->initalised = true;
}


int32_t adc_read(adc_channel_t *channel)
{
	int32_t r;
	struct adc_t *adc = channel->adc;

	SDADC_ChannelSelect(adc->base, channel->number);

	SDADC_SoftwareStartConv(adc->base);
	while (SDADC_GetFlagStatus(adc->base, SDADC_FLAG_REOC) == RESET)
	{}

	r = SDADC_GetConversionValue(adc->base);
	if (adc->SDADC_AINStructure[channel->conf].SDADC_InputMode == SDADC_InputMode_SEZeroReference)
		r += 32768;

	return r;
}


void adc_channel_init(adc_channel_t *channel)
{
	// if the adc is not initialised yet init that
	struct adc_t *adc = channel->adc;
	if (!adc->initalised)
		adc_init(adc);
	
	// set analog input pin
	gpio_init_pin(channel->pin);

	// link this channel to its config
	SDADC_ChannelConfig(adc->base, channel->number, channel->conf);
}

