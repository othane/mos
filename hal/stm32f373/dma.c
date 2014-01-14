/**
 * @file dma.c
 *
 * @brief implement the dma module for the stm32f373
 *
 * @author OT
 *
 * @date Jan 2014
 *
 */

#include <stm32f37x_conf.h>
#include "hal.h"
#include "dma_hw.h"


static dma_t *dma1_irq_list[7] = {NULL,};
static dma_t *dma2_irq_list[5] = {NULL,};

static unsigned int dma_irq(dma_t *dma)
{
	// map dma->channel to irq
	switch ((uint32_t)dma->channel)
	{
		case (uint32_t)DMA1_Channel1:
			dma1_irq_list[0] = dma;
			return DMA1_Channel1_IRQn;
		case (uint32_t)DMA1_Channel2:
			dma1_irq_list[1] = dma;
			return DMA1_Channel2_IRQn;
		case (uint32_t)DMA1_Channel3:
			dma1_irq_list[2] = dma;
			return DMA1_Channel3_IRQn;
		case (uint32_t)DMA1_Channel4:
			dma1_irq_list[3] = dma;
			return DMA1_Channel4_IRQn;
		case (uint32_t)DMA1_Channel5:
			dma1_irq_list[4] = dma;
			return DMA1_Channel5_IRQn;
		case (uint32_t)DMA1_Channel6:
			dma1_irq_list[5] = dma;
			return DMA1_Channel6_IRQn;
		case (uint32_t)DMA1_Channel7:
			dma1_irq_list[6] = dma;
			return DMA1_Channel7_IRQn;
		case (uint32_t)DMA2_Channel1:
			dma2_irq_list[0] = dma;
			return DMA2_Channel1_IRQn;
		case (uint32_t)DMA2_Channel2:
			dma2_irq_list[1] = dma;
			return DMA2_Channel2_IRQn;
		case (uint32_t)DMA2_Channel3:
			dma2_irq_list[2] = dma;
			return DMA2_Channel3_IRQn;
		case (uint32_t)DMA2_Channel4:
			dma2_irq_list[3] = dma;
			return DMA2_Channel4_IRQn;
		case (uint32_t)DMA2_Channel5:
			dma2_irq_list[4] = dma;
			return DMA2_Channel5_IRQn;
		default:
			///todo error !
			return DMA1_Channel1_IRQn;
	}
}

static void dma_clear_gl(dma_t *dma)
{
	switch ((uint32_t)dma->channel)
	{
		case (uint32_t)DMA1_Channel1:
    		return DMA_ClearITPendingBit(DMA1_IT_GL1);
		case (uint32_t)DMA1_Channel2:
    		return DMA_ClearITPendingBit(DMA1_IT_GL2);
		case (uint32_t)DMA1_Channel3:
    		return DMA_ClearITPendingBit(DMA1_IT_GL3);
		case (uint32_t)DMA1_Channel4:
    		return DMA_ClearITPendingBit(DMA1_IT_GL4);
		case (uint32_t)DMA1_Channel5:
    		return DMA_ClearITPendingBit(DMA1_IT_GL5);
		case (uint32_t)DMA1_Channel6:
    		return DMA_ClearITPendingBit(DMA1_IT_GL6);
		case (uint32_t)DMA1_Channel7:
    		return DMA_ClearITPendingBit(DMA1_IT_GL7);
		case (uint32_t)DMA2_Channel1:
    		return DMA_ClearITPendingBit(DMA2_IT_GL1);
		case (uint32_t)DMA2_Channel2:
    		return DMA_ClearITPendingBit(DMA2_IT_GL2);
		case (uint32_t)DMA2_Channel3:
    		return DMA_ClearITPendingBit(DMA2_IT_GL3);
		case (uint32_t)DMA2_Channel4:
    		return DMA_ClearITPendingBit(DMA2_IT_GL4);
		case (uint32_t)DMA2_Channel5:
    		return DMA_ClearITPendingBit(DMA2_IT_GL5);
		default:
			///@todo error !
			return;
	}
}

static void dma_irq_handler(dma_t *dma)
{
	dma_request_t *req;

	if (dma == NULL || dma->reqs == NULL)
		///@todo interrupt for dma that is not enabled !
		return;

	DMA_ITConfig(dma->channel, DMA_IT_TC, DISABLE);
	DMA_Cmd(dma->channel, DISABLE);

	req = dma->reqs;
	dma->reqs = NULL; // free the dma before complete so complete can sched another
	if (req->complete != NULL)
		req->complete(req, req->complete_param);
}

void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1))
	{
    	DMA_ClearITPendingBit(DMA1_IT_GL1);
		dma_irq_handler(dma1_irq_list[0]);
	}
}

void DMA1_Channel2_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC2))
	{
		DMA_ClearITPendingBit(DMA1_IT_GL2);
		dma_irq_handler(dma1_irq_list[1]);
	}
}

void DMA1_Channel3_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC3))
	{
		DMA_ClearITPendingBit(DMA1_IT_GL3);
		dma_irq_handler(dma1_irq_list[2]);
	}
}

void DMA1_Channel4_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC4))
	{
		DMA_ClearITPendingBit(DMA1_IT_GL4);
		dma_irq_handler(dma1_irq_list[3]);
	}
}

void DMA1_Channel5_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC5))
	{
		DMA_ClearITPendingBit(DMA1_IT_GL5);
		dma_irq_handler(dma1_irq_list[4]);
	}
}

void DMA1_Channel6_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC6))
	{
		DMA_ClearITPendingBit(DMA1_IT_GL6);
		dma_irq_handler(dma1_irq_list[5]);
	}
}

void DMA1_Channel7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC7))
	{
		DMA_ClearITPendingBit(DMA1_IT_GL7);
		dma_irq_handler(dma1_irq_list[6]);
	}
}

void DMA2_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_IT_TC1))
	{
		DMA_ClearITPendingBit(DMA2_IT_GL1);
		dma_irq_handler(dma2_irq_list[0]);
	}
}

void DMA2_Channel2_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_IT_TC2))
	{
		DMA_ClearITPendingBit(DMA2_IT_GL2);
		dma_irq_handler(dma2_irq_list[1]);
	}
}

void DMA2_Channel3_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_IT_TC3))
	{
		DMA_ClearITPendingBit(DMA2_IT_GL3);
		dma_irq_handler(dma2_irq_list[2]);
	}
}

void DMA2_Channel4_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_IT_TC4))
	{
		DMA_ClearITPendingBit(DMA2_IT_GL4);
		dma_irq_handler(dma2_irq_list[3]);
	}
}

void DMA2_Channel5_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_IT_TC5))
	{
		DMA_ClearITPendingBit(DMA2_IT_GL5);
		dma_irq_handler(dma2_irq_list[4]);
	}
}


static dma_request_t dma_memcpy_req;

static dma_memcpy_complete_event_t dma_memcpy_complete = NULL;

static void memcpy_complete(dma_request_t *req, void *param)
{
	if (req == &dma_memcpy_req && dma_memcpy_complete != NULL)
	{
		void *src = (void *)req->st_dma_init.DMA_PeripheralBaseAddr;
		void *dst = (void *)req->st_dma_init.DMA_MemoryBaseAddr;
		int len =  req->st_dma_init.DMA_BufferSize;
		dma_memcpy_complete(req->dma, dst, src, len);
	}
}

void dma_memcpy(dma_t *dma, void *dst, void *src, int len, dma_memcpy_complete_event_t complete)
{
	dma_request_t *req = &dma_memcpy_req;

	dma_memcpy_complete = complete;

	req->complete = memcpy_complete;
	req->complete_param = NULL;
	req->dma = dma;

	req->st_dma_init.DMA_PeripheralBaseAddr = (uint32_t)src;
	req->st_dma_init.DMA_MemoryBaseAddr = (uint32_t)dst;
	req->st_dma_init.DMA_DIR = DMA_DIR_PeripheralSRC;
	req->st_dma_init.DMA_BufferSize = len;
	req->st_dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	req->st_dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
	req->st_dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	req->st_dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	req->st_dma_init.DMA_Mode = DMA_Mode_Normal;
	req->st_dma_init.DMA_Priority = DMA_Priority_Low;
	req->st_dma_init.DMA_M2M = DMA_M2M_Enable;

	dma_request(req);
}

void dma_request(dma_request_t *req)
{
	dma_t *dma = req->dma;

	///@todo queue a list of requests (for now just 1 at a time please)
	if (dma->reqs != NULL)
		///@todo error dma busy !
		return;
	dma->reqs = req;

	DMA_Init(dma->channel, &req->st_dma_init);
	dma_clear_gl(dma);
	DMA_ITConfig(dma->channel, DMA_IT_TC, ENABLE);
	DMA_Cmd(dma->channel, ENABLE);
}

void dma_init(dma_t *dma)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	if (dma == NULL)
		return;

	dma->reqs = NULL;

	// enable clocks
	switch ((uint32_t)dma->channel)
	{
		case (uint32_t)DMA1_Channel1:
		case (uint32_t)DMA1_Channel2:
		case (uint32_t)DMA1_Channel3:
		case (uint32_t)DMA1_Channel4:
		case (uint32_t)DMA1_Channel5:
		case (uint32_t)DMA1_Channel6:
		case (uint32_t)DMA1_Channel7:
			RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
			break;
		case (uint32_t)DMA2_Channel1:
		case (uint32_t)DMA2_Channel2:
		case (uint32_t)DMA2_Channel3:
		case (uint32_t)DMA2_Channel4:
		case (uint32_t)DMA2_Channel5:
			RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
			break;
		default:
			/// todo error !
			break;
	}

	// enable nvic
	NVIC_InitStructure.NVIC_IRQChannel = dma_irq(dma);
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

