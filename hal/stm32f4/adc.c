/**
 * @file adc.c
 *
 * @brief implement the adc module for the stm32f4
 *
 * @author OT
 *
 * @date Jan 2014
 *
 */


#include <stm32f4xx_conf.h>
#include "hal.h"
#include "gpio_hw.h"
#include "adc_hw.h"


static uint32_t adc_apb2_periph(ADC_TypeDef *base)
{
	switch ((uint32_t)base)
	{
		case (uint32_t)ADC1:
			return RCC_APB2Periph_ADC1;
		case (uint32_t)ADC2:
			return RCC_APB2Periph_ADC2;
		case (uint32_t)ADC3:
			return RCC_APB2Periph_ADC3;
		default:
			return RCC_APB2Periph_ADC1;
	}
}


extern adc_channel_t *__sys_temperature_sensor;
static void adc_init(adc_t *adc)
{
	ADC_CommonInitTypeDef common_init =
	{
		.ADC_Mode = ADC_Mode_Independent,
		.ADC_Prescaler = ADC_Prescaler_Div4, // 84 / 4 = 21MHz is the fastest option below fADC_max = 36MHz
		.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled,
		.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_10Cycles,
	};

	// enable adc digital interface clk so we can talk to the regs
	RCC_APB2PeriphClockCmd(adc_apb2_periph(adc->base), ENABLE);

	// init the adc
	ADC_CommonInit(&common_init);

	// int the dma if given
	if (adc->dma)
		dma_init(adc->dma);

	ADC_Cmd(adc->base, ENABLE);
	if (__sys_temperature_sensor != NULL)
		ADC_TempSensorVrefintCmd(ENABLE);

	adc->initalised = true;
}


void adc_channel_init(adc_channel_t *ch)
{
	// if the adc is not initialised yet init that
	struct adc_t *adc = ch->adc;
	if (!adc->initalised)
		adc_init(adc);

	// set analog input pin
	gpio_init_pin(ch->pin);
}


static uint32_t __NULL;
static void adc_dma_cfg(adc_t *adc, dma_request_t *dma_req, void *buf, int count)
{
	DMA_InitTypeDef *adc_cfg = &dma_req->st_dma_init;
	dma_t *dma = dma_req->dma;

	adc_cfg->DMA_Channel = dma->channel;
	adc_cfg->DMA_PeripheralBaseAddr = (uint32_t)&adc->base->DR;
	adc_cfg->DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	adc_cfg->DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;

	adc_cfg->DMA_Memory0BaseAddr = (uint32_t)buf;
	adc_cfg->DMA_MemoryInc = DMA_MemoryInc_Enable;
	if (buf == NULL)
	{
		// this NULL is a possible security hole as multiple io may read/write to
		// this, it is only really for dbg and a real buffer should really be used
		// at all times
		adc_cfg->DMA_Memory0BaseAddr = (uint32_t)&__NULL;
		adc_cfg->DMA_MemoryInc = DMA_MemoryInc_Disable;
	}
	adc_cfg->DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	adc_cfg->DMA_Mode = adc->dma->circ? DMA_Mode_Circular: DMA_Mode_Normal;
	adc_cfg->DMA_Priority = DMA_Priority_High;
	adc_cfg->DMA_FIFOMode = DMA_FIFOMode_Disable;
	adc_cfg->DMA_MemoryBurst = DMA_MemoryBurst_Single;
	adc_cfg->DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	adc_cfg->DMA_BufferSize = count;
	adc_cfg->DMA_DIR = DMA_DIR_PeripheralToMemory;
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
		ADC_InitTypeDef init =
		{
			.ADC_Resolution = ADC_Resolution_12b,
			.ADC_ScanConvMode = DISABLE,
			.ADC_ContinuousConvMode = DISABLE,
			.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None,
			.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1, // this is dont care if ext trig is disable above
			.ADC_DataAlign = ADC_DataAlign_Right,
			.ADC_NbrOfConversion = 1,
		};

		// stop the DMA, then disable continuous mode, and disable the external
		// trigger so we dont get set off again
		ADC_DMACmd(adc->base, DISABLE);
		ADC_Init(adc->base, &init);
		ADC_GetConversionValue(adc->base); // clear EOC flag in case of overrun
		ADC_ClearFlag(adc->base, ADC_FLAG_OVR);
	}

	sys_leave_critical_section();

	if (req->dma->isr_status & 0x20)
		count = ch->count;
	else if (req->dma->isr_status & 0x10)
	{
		if (ch->count & 0x01)
			// if count is odd isr rounds up
			count = (ch->count >> 1) + 1;
		else
			count = (ch->count >> 1);
	}

	cb = ch->complete;
	if (cb)
		cb(ch, ch->buf, count, ch->complete_param);
}


void adc_trace(adc_channel_t *ch, uint16_t *dst, int count, int trigger, adc_trace_complete_t cb, void *param)
{
	adc_t *adc = ch->adc;
	ADC_InitTypeDef init =
	{
		.ADC_Resolution = ADC_Resolution_12b,
		.ADC_ScanConvMode = DISABLE,
		.ADC_DataAlign = ADC_DataAlign_Right,
		.ADC_NbrOfConversion = 1,
	};

	sys_enter_critical_section();

	// setup dma
	if (adc->dma == NULL)
		///@todo error current implementation does not support interrupts so we need a dma
		goto done;
	dma_cancel(adc->dma); // cancel any pending/running dma
	adc->dma_req.complete = adc_dma_complete;
	ch->buf = dst;
	ch->count = count;
	ch->complete = cb;
	ch->complete_param = param;
	adc->dma_req.complete_param = ch;
	adc->dma_req.dma = adc->dma;
	adc_dma_cfg(adc, &adc->dma_req, (void *)dst, count);
	dma_request(&adc->dma_req);
	ADC_DMACmd(adc->base, ENABLE);

	// setup the trigger that keeps the adc running count times, if no trigger is given
	// we use continuous mode to trigger it count times
	ADC_GetConversionValue(adc->base); // clear EOC flag to ensure dma will be triggered (it needs an edge !)
	if (trigger)
	{
		init.ADC_ContinuousConvMode = !adc->trigger.cont;
		init.ADC_ExternalTrigConvEdge = adc->trigger.type;
		init.ADC_ExternalTrigConv = adc->trigger.source;
	}
	else
	{
		init.ADC_ContinuousConvMode = !adc->trigger.cont;
		init.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
		init.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1; // this is dont care if ext trig is disable above
	}

	// init the adc
	ADC_Init(adc->base, &init);
	ADC_DMARequestAfterLastTransferCmd(adc->base, ENABLE);   ///@todo it would be better to change this to DISABLE if not in circ mode (atm we just clear the OVR flag)

	// init the adc channel as the only channel (we dont support sequences atm
	// we just do the one channel _count_ times)
	ADC_RegularChannelConfig(adc->base, ch->number, 1, ch->sample_time);

done:
	sys_leave_critical_section();

	// if we don't have a trigger start the adc manually
	if (!trigger)
		ADC_SoftwareStartConv(adc->base);
}


void adc_cancel_trace(adc_channel_t *ch)
{
	adc_t *adc = ch->adc;
	ADC_InitTypeDef init =
	{
		.ADC_Resolution = ADC_Resolution_12b,
		.ADC_ScanConvMode = DISABLE,
		.ADC_ContinuousConvMode = DISABLE,
		.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None,
		.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1, // this is dont care if ext trig is disable above
		.ADC_DataAlign = ADC_DataAlign_Right,
		.ADC_NbrOfConversion = 1,
	};

	// cancel dma
	sys_enter_critical_section();
	if (adc->dma == NULL)
		///@todo error current implementation does not support interrupts so we need a dma
		return;
	ADC_DMACmd(adc->base, DISABLE);
	dma_cancel(adc->dma); // cancel any pending/running dma
	
	// clean up like we would on adc_dma_complete and just reset everything to default state
	ADC_Init(adc->base, &init);
	ADC_GetConversionValue(adc->base); // clear EOC flag in case of overrun
	ADC_ClearFlag(adc->base, ADC_FLAG_OVR);

	sys_leave_critical_section();
}


int32_t adc_read(adc_channel_t *ch)
{
	int32_t ret;
	struct adc_t *adc = ch->adc;
	ADC_InitTypeDef init =
	{
		.ADC_Resolution = ADC_Resolution_12b,
		.ADC_ScanConvMode = DISABLE,
		.ADC_ContinuousConvMode = DISABLE,
		.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None,
		.ADC_DataAlign = ADC_DataAlign_Right,
		.ADC_NbrOfConversion = 1,
	};
	ADC_DMACmd(adc->base, DISABLE);
	ADC_Init(adc->base, &init);

	// init the adc channel as the only channel
	ADC_RegularChannelConfig(adc->base, ch->number, 1, ch->sample_time);

	sys_enter_critical_section();
	ADC_ClearFlag(adc->base, ADC_FLAG_EOC);
	ADC_SoftwareStartConv(adc->base);
	sys_leave_critical_section();
	while (ADC_GetFlagStatus(adc->base, ADC_FLAG_EOC) == RESET)
	{}

	ret = (int32_t)ADC_GetConversionValue(adc->base);
	return ret;
}

