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
	uint32_t sadc_clk_div = adc->sadc_clk_div;

	// set adc clk to 6MHz
	RCC_APB2PeriphClockCmd(adc_apb2_periph(adc->base), ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_SDADCAnalogCmd(adc_pwr_periph(adc->base), ENABLE);
	if (sadc_clk_div)
		RCC_SDADCCLKConfig(sadc_clk_div);
	else
		// default to max speed if not specified
		RCC_SDADCCLKConfig(RCC_SDADCCLK_SYSCLK_Div12);

	// int the dma if given
	if (adc->dma)
		dma_init(adc->dma);

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

	adc->initalised = true;
}


static uint32_t __NULL;
static void adc_dma_cfg(adc_t *adc, dma_request_t *dma_req, void *buf, int count)
{
	DMA_InitTypeDef *adc_cfg = &dma_req->st_dma_init;

	///@todo if sync is on then Base needs to be JDATA12R || JDATA13R and word size = DMA_PeripheralDataSize_Word
	adc_cfg->DMA_PeripheralBaseAddr = (uint32_t)&adc->base->JDATAR;
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
	SDADC_DMAConfig(adc->base, SDADC_DMATransfer_Injected, ENABLE);
	dma_request(&adc->dma_req);

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
	sys_leave_critical_section();

	// if we don't have a trigger start the adc manually
	if (!trigger)
		SDADC_SoftwareStartInjectedConv(adc->base);
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
	gpio_init_pin(channel->pin_ref);

	// link this channel to its config
	SDADC_ChannelConfig(adc->base, channel->number, channel->conf);
}

