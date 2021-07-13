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


void ADC_ExternalTrigConvConfig(ADC_TypeDef* ADCx, uint32_t ADC_ExternalTrigConv)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_EXT_TRIG(ADC_ExternalTrigConv));
  /* Get the old register value */
  tmpreg = ADCx->CR2;
  /* Clear the old external event selection for injected group */
  tmpreg &= (uint32_t) (~ADC_CR2_EXTSEL);
  /* Set the external event selection for injected group */
  tmpreg |= ADC_ExternalTrigConv;
  /* Store the new register value */
  ADCx->CR2 = tmpreg;
}


static uint32_t adc_apb2_periph(void *base)
{
	switch ((uint32_t)base)
	{
		case (uint32_t)SDADC1:
			return RCC_APB2Periph_SDADC1;
		case (uint32_t)SDADC2:
			return RCC_APB2Periph_SDADC2;
		case (uint32_t)SDADC3:
			return RCC_APB2Periph_SDADC3;
		case (uint32_t)ADC1:
			return RCC_APB2Periph_ADC1;
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


static enum adc_type adc_get_type(void *base)
{
	switch ((int)base)
	{
		case (int)ADC1:
			return ADC;
		case (int)SDADC1:
		case (int)SDADC2:
		case (int)SDADC3:
		default:
			return SDADC;
	}
}


static void adc_init(adc_t *adc)
{
	uint32_t adc_clk_div = adc->adc_clk_div;

	adc->type = adc_get_type(adc->base);

	// setup adc clks
	RCC_APB2PeriphClockCmd(adc_apb2_periph(adc->base), ENABLE);

	// int the dma if given
	if (adc->dma)
		dma_init(adc->dma);

	switch (adc->type)
	{
		case SDADC:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

			PWR_SDADCAnalogCmd(adc_pwr_periph(adc->base), ENABLE);
			if (adc_clk_div)
				RCC_SDADCCLKConfig(adc_clk_div);
			else
				// default to max speed if not specified
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

			// option to allow adc's to be sync'ed
			SDADC_InjectedSynchroSDADC1(adc->base, adc->trigger.sync_adc1);

			SDADC_InitModeCmd(adc->base, DISABLE);

			// calibrate
			SDADC_CalibrationSequenceConfig(adc->base, SDADC_CalibrationSequence_3);
			SDADC_StartCalibration(adc->base);
			while (SDADC_GetFlagStatus(adc->base, SDADC_FLAG_EOCAL) == RESET)
			{}
			SDADC_ClearFlag(adc->base, SDADC_FLAG_EOCAL);
			break;
		case ADC:
			if (adc_clk_div)
				RCC_ADCCLKConfig(adc_clk_div);
			else
				// default to max speed if not specified
				RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 72MHz/6 = 12Mhz * 14clks/conv = 1.17us/conv (fastest speed possible at APB2=72Mhz)

			ADC_InitTypeDef init =
			{
				.ADC_ScanConvMode = DISABLE,
				.ADC_ContinuousConvMode = !adc->trigger.cont,
				.ADC_ExternalTrigConv = adc->trigger.source,
				.ADC_DataAlign = ADC_DataAlign_Right,
				.ADC_NbrOfChannel = 1,
			};

			// init the adc
			ADC_DeInit(adc->base);
			ADC_Init(adc->base, &init);
			ADC_Cmd(adc->base, ENABLE);
			ADC_TempSensorVrefintCmd(ENABLE);

			// calibration
			ADC_ResetCalibration(adc->base);
			while(ADC_GetResetCalibrationStatus(adc->base));
			ADC_StartCalibration(adc->base);
			while(ADC_GetCalibrationStatus(adc->base));

			// always use the external trigger so we can start the conversion
			// using ADC_SoftwareStartConv or every ADC_Cmd(XXX, ENABLE) will
			// start the conversion
			ADC_ExternalTrigConvCmd(adc->base, ENABLE);
			ADC_Cmd(adc->base, ENABLE);

			break;
	}

	adc->initalised = true;
}


static uint32_t __NULL;
static void adc_dma_cfg(adc_t *adc, dma_request_t *dma_req, void *buf, int count)
{
	DMA_InitTypeDef *adc_cfg = &dma_req->st_dma_init;

	///@todo if sync is on then Base needs to be JDATA12R || JDATA13R and word size = DMA_PeripheralDataSize_Word
	if (adc->type == SDADC)
		adc_cfg->DMA_PeripheralBaseAddr = (uint32_t)&((SDADC_TypeDef *)adc->base)->JDATAR;
	else
		adc_cfg->DMA_PeripheralBaseAddr = (uint32_t)&((ADC_TypeDef *)adc->base)->DR;
	adc_cfg->DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	adc_cfg->DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;

	adc_cfg->DMA_MemoryBaseAddr = (uint32_t)buf;
	adc_cfg->DMA_MemoryInc = DMA_MemoryInc_Enable;
	if (buf == NULL)
	{
		// this NULL is a possible security hole as multiple io may read/write to
		// this, it is only really for dbg and a real buffer should really be used
		// at all times
		adc_cfg->DMA_MemoryBaseAddr = (uint32_t)&__NULL;
		adc_cfg->DMA_MemoryInc = DMA_MemoryInc_Disable;
	}
	adc_cfg->DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	adc_cfg->DMA_Mode = adc->dma->circ? DMA_Mode_Circular: DMA_Mode_Normal;
	adc_cfg->DMA_Priority = DMA_Priority_High;
	adc_cfg->DMA_M2M = DMA_M2M_Disable;
	adc_cfg->DMA_BufferSize = count;
	adc_cfg->DMA_DIR = DMA_DIR_PeripheralSRC;
}


static void adc_dma_complete(dma_request_t *req, void *param)
{
	adc_channel_t *ch = (adc_channel_t *)param;
	adc_t * adc = ch->adc;
	adc_trace_complete_t cb;
	int count;

	sys_enter_critical_section();

	switch (adc->type)
	{
		case SDADC:
			// if we are in circular mode just keep going
			if (!adc->dma->circ)
			{
				// stop the DMA, and turn off continuous mode, we are done
				SDADC_DMAConfig(ch->adc->base, SDADC_DMATransfer_Injected, DISABLE);

				// stop continous mode
				SDADC_InjectedContinuousModeCmd(ch->adc->base, DISABLE);

				// reset the trigger source
				SDADC_InitModeCmd(adc->base, ENABLE);
				while (SDADC_GetFlagStatus(adc->base, SDADC_FLAG_INITRDY) == RESET)
				{}
				SDADC_ExternalTrigInjectedConvEdgeConfig(adc->base, SDADC_ExternalTrigInjecConvEdge_None);
				SDADC_InitModeCmd(adc->base, DISABLE);

			}
			///@todo handle any overruns (we should really handle this in a isr
			/// but for now we can just clear the flag and hope the dma still completes)
			SDADC_ClearITPendingBit(adc->base, SDADC_IT_JOVR);
			break;
		case ADC:
			// if we are in circular mode just keep going
			if (!adc->dma->circ)
			{
				// stop the DMA, and turn off continuous mode, we are done
				ADC_DMACmd(ch->adc->base, DISABLE);
				ADC_ContinuousModeCmd(adc->base, DISABLE); // stop continuous mode
				ADC_ExternalTrigConvConfig(adc->base, ADC_ExternalTrigConv_None); // reset the trigger source

			}
			// clear EOC flag in case of overrun
			ADC_GetConversionValue(adc->base);
			ADC_ClearFlag(adc->base, ADC_FLAG_EOC);
			break;
	}

	sys_leave_critical_section();

	if (req->dma->isr_status & 2)
		count = ch->count;
	else if (ch->count & 0x01)
		// if count is odd isr rounds up
		count = (ch->count >> 1) + 1;
	else
		count = (ch->count >> 1);

	cb = ch->complete;
	if (cb)
		cb(ch, ch->buf, count, ch->complete_param);
}


void adc_trace(adc_channel_t *ch, volatile int16_t *dst, int count, int trigger, adc_trace_complete_t cb, void *param)
{
	adc_t *adc = ch->adc;
	uint32_t num;

	// setup dma
	sys_enter_critical_section();
	if (adc->dma == NULL)
		///@todo error current implementation does not support interrupts so we need a dma
		return;
	dma_cancel(adc->dma); // cancel any pending/running dma
	adc->dma_req.complete = adc_dma_complete;
	ch->buf = dst;
	ch->count = count;
	ch->complete = cb;
	ch->complete_param = param;
	adc->dma_req.complete_param = ch;
	adc->dma_req.dma = adc->dma;
	adc_dma_cfg(adc, &adc->dma_req, (void *)dst, count);
	if (adc->type == SDADC)
		SDADC_DMAConfig(adc->base, SDADC_DMATransfer_Injected, ENABLE);
	else
		ADC_DMACmd(adc->base, ENABLE);
	dma_request(&adc->dma_req);

	switch (adc->type)
	{
		case SDADC:
			// use injected mode for all conversions so we can use a trigger if given
			SDADC_InjectedChannelSelect(adc->base, ch->number);
			SDADC_GetInjectedConversionValue(adc->base, &num); // clear JEOCF flag to ensure dma will be triggered (it needs an edge !)

			// the following set-up(s) requires init mode
			SDADC_InitModeCmd(adc->base, ENABLE);
			while (SDADC_GetFlagStatus(adc->base, SDADC_FLAG_INITRDY) == RESET)
			{}

			// setup the trigger that keeps the adc running count times, if no trigger is given
			// we use continuous mode to trigger it count times .. we use the fast mode when
			// continuous mode is set since we assume a single channel for trace atm
			if (trigger)
			{
				SDADC_InjectedContinuousModeCmd(adc->base, !adc->trigger.cont);
				SDADC_ExternalTrigInjectedConvConfig(adc->base, adc->trigger.source);
				SDADC_ExternalTrigInjectedConvEdgeConfig(adc->base, adc->trigger.type);
				SDADC_FastConversionCmd(adc->base, !adc->trigger.cont);
			}
			else
			{
				// we have no trigger so use continuous mode with the dma
				SDADC_InjectedContinuousModeCmd(adc->base, ENABLE);
				SDADC_ExternalTrigInjectedConvEdgeConfig(adc->base, SDADC_ExternalTrigInjecConvEdge_None);
				SDADC_FastConversionCmd(adc->base, ENABLE);
			}

			// close init mode
			SDADC_InitModeCmd(adc->base, DISABLE);
			break;

		case ADC:
			// setup regular channel
			ADC_RegularChannelConfig(adc->base, ch->number, 1, ch->sample_time);

			// enable the trigger if we have one else just get the adc ready to
			// go on the ADC_SoftwareStartConv
			if (trigger)
			{
				ADC_ContinuousModeCmd(adc->base, !adc->trigger.cont);
				ADC_ExternalTrigConvConfig(adc->base, adc->trigger.source);
			}
			else
			{
				ADC_ContinuousModeCmd(adc->base, ENABLE);
				ADC_ExternalTrigConvConfig(adc->base, ADC_ExternalTrigConv_None);
			}
			ADC_Cmd(adc->base, ENABLE);
			break;
	}

	sys_leave_critical_section();

	// if we don't have a trigger start the adc manually
	if (!trigger) {
		if (adc->type == SDADC)
			SDADC_SoftwareStartInjectedConv(adc->base);
		else
			ADC_SoftwareStartConv(adc->base);
	}
}


void adc_cancel_trace(adc_channel_t *ch)
{
	adc_t *adc = ch->adc;

	// cancel dma
	sys_enter_critical_section();
	if (adc->dma == NULL)
		///@todo error current implementation does not support interrupts so we need a dma
		return;
	dma_cancel(adc->dma); // cancel any pending/running dma

	// clean up like we would on adc_dma_complete and just reset everything to default state
	switch (adc->type)
	{
		case SDADC:
			SDADC_DMAConfig(ch->adc->base, SDADC_DMATransfer_Injected, DISABLE); // disable ADC dma connection
			SDADC_InjectedContinuousModeCmd(ch->adc->base, DISABLE); // stop continuous mode
			SDADC_InitModeCmd(adc->base, ENABLE); // reset the trigger source
			while (SDADC_GetFlagStatus(adc->base, SDADC_FLAG_INITRDY) == RESET)
			{}
			SDADC_ExternalTrigInjectedConvEdgeConfig(adc->base, SDADC_ExternalTrigInjecConvEdge_None);
			SDADC_InitModeCmd(adc->base, DISABLE);
			SDADC_ClearITPendingBit(adc->base, SDADC_IT_JOVR);
			break;
		case ADC:
			ADC_DMACmd(ch->adc->base, DISABLE); // disable ADC dma connection
			ADC_ContinuousModeCmd(adc->base, DISABLE); // stop continuous mode
			ADC_ExternalTrigConvConfig(adc->base, ADC_ExternalTrigConv_None); // reset the trigger source

			// clear EOC flag in case of overrun
			ADC_GetConversionValue(adc->base);
			ADC_ClearFlag(adc->base, ADC_FLAG_EOC);
			break;
	}

	sys_leave_critical_section();
}


int32_t adc_read(adc_channel_t *channel)
{
	int32_t r;
	struct adc_t *adc = channel->adc;

	switch (adc->type)
	{
		case SDADC:
			SDADC_ChannelSelect(adc->base, channel->number);

			SDADC_SoftwareStartConv(adc->base);
			while (SDADC_GetFlagStatus(adc->base, SDADC_FLAG_REOC) == RESET)
			{}

			r = SDADC_GetConversionValue(adc->base);
			if (adc->SDADC_AINStructure[channel->conf].SDADC_InputMode == SDADC_InputMode_SEZeroReference)
				r += 32768;
			break;
		case ADC:
			ADC_DMACmd(adc->base, DISABLE);
			ADC_ContinuousModeCmd(adc->base, DISABLE);
			ADC_ExternalTrigConvConfig(adc->base, ADC_ExternalTrigConv_None);

			ADC_RegularChannelConfig(adc->base, channel->number, 1, channel->sample_time);

			sys_enter_critical_section();
			ADC_ClearFlag(adc->base, ADC_FLAG_EOC);
			ADC_SoftwareStartConv(adc->base);
			sys_leave_critical_section();
			while (ADC_GetFlagStatus(adc->base, ADC_FLAG_EOC) == RESET)
			{}

			r = (int32_t)ADC_GetConversionValue(adc->base);
			break;
	}
	return r;
}


#define ABS(x) ((x < 0)? -(x): (x))
void adc_set_gain(adc_channel_t *channel, uint8_t gain)
{
	struct adc_t *adc = channel->adc;
	struct {
		int g;
		uint32_t regval;
	} gains[] = {
		{.g = 1, .regval = SDADC_Gain_1},
		{.g = 2, .regval = SDADC_Gain_2},
		{.g = 4, .regval = SDADC_Gain_4},
		{.g = 8, .regval = SDADC_Gain_8},
		{.g = 16, .regval = SDADC_Gain_16},
		{.g = 32, .regval = SDADC_Gain_32},
	};
	int min_err = 1000, min_err_idx = 0, n;

	// select the nearest gain
	for (n = 0; n < 6; n++)
	{
		int err = ABS(gains[n].g - (int)gain);
		if (err < min_err)
		{
			min_err_idx = n;
			min_err = err;
		}
	}
	///@todo maybe we could add a post adc software gain to make this exact
	adc->SDADC_AINStructure[channel->conf].SDADC_Gain = gains[min_err_idx].regval;

	// update the SDADC_AINStructure gain
	SDADC_InitModeCmd(adc->base, ENABLE);
	while (SDADC_GetFlagStatus(adc->base, SDADC_FLAG_INITRDY) == RESET)
	{}
	SDADC_AINInit(adc->base, channel->conf, &adc->SDADC_AINStructure[channel->conf]);
	SDADC_InitModeCmd(adc->base, DISABLE);
}


void adc_channel_init(adc_channel_t *channel)
{
	// if the adc is not initialised yet init that
	struct adc_t *adc = channel->adc;
	if (!adc->initalised)
		adc_init(adc);

	// set analog input pin
	gpio_init_pin(channel->pin);
	if (adc->type == SDADC)
	{
		gpio_init_pin(channel->pin_ref);

		// link this channel to its config
		SDADC_ChannelConfig(adc->base, channel->number, channel->conf);
	}
}

