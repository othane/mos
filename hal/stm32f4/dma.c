/**
 * @file dma.c
 *
 * @brief implement the dma module for the stm32f4
 *
 * @author OT
 *
 * @date Jan 2014
 *
 */

#include <stm32f4xx_conf.h>
#include "hal.h"
#include "dma_hw.h"


static dma_t *dma1_irq_list[8] = {NULL,};
static dma_t *dma2_irq_list[8] = {NULL,};

static unsigned int dma_irq(dma_t *dma)
{
	// map dma->stream to irq
	switch ((uint32_t)dma->stream)
	{
		case (uint32_t)DMA1_Stream0:
			dma1_irq_list[0] = dma;
			return DMA1_Stream0_IRQn;
		case (uint32_t)DMA1_Stream1:
			dma1_irq_list[1] = dma;
			return DMA1_Stream1_IRQn;
		case (uint32_t)DMA1_Stream2:
			dma1_irq_list[2] = dma;
			return DMA1_Stream2_IRQn;
		case (uint32_t)DMA1_Stream3:
			dma1_irq_list[3] = dma;
			return DMA1_Stream3_IRQn;
		case (uint32_t)DMA1_Stream4:
			dma1_irq_list[4] = dma;
			return DMA1_Stream4_IRQn;
		case (uint32_t)DMA1_Stream5:
			dma1_irq_list[5] = dma;
			return DMA1_Stream5_IRQn;
		case (uint32_t)DMA1_Stream6:
			dma1_irq_list[6] = dma;
			return DMA1_Stream6_IRQn;
		case (uint32_t)DMA1_Stream7:
			dma1_irq_list[7] = dma;
			return DMA1_Stream7_IRQn;

		case (uint32_t)DMA2_Stream0:
			dma2_irq_list[0] = dma;
			return DMA2_Stream0_IRQn;
		case (uint32_t)DMA2_Stream1:
			dma2_irq_list[1] = dma;
			return DMA2_Stream1_IRQn;
		case (uint32_t)DMA2_Stream2:
			dma2_irq_list[2] = dma;
			return DMA2_Stream2_IRQn;
		case (uint32_t)DMA2_Stream3:
			dma2_irq_list[3] = dma;
			return DMA2_Stream3_IRQn;
		case (uint32_t)DMA2_Stream4:
			dma2_irq_list[4] = dma;
			return DMA2_Stream4_IRQn;
		case (uint32_t)DMA2_Stream5:
			dma2_irq_list[5] = dma;
			return DMA2_Stream5_IRQn;
		case (uint32_t)DMA2_Stream6:
			dma2_irq_list[6] = dma;
			return DMA2_Stream6_IRQn;
		case (uint32_t)DMA2_Stream7:
			dma2_irq_list[7] = dma;
			return DMA2_Stream7_IRQn;

		default:
			///todo error !
			return DMA1_Stream1_IRQn;
	}
}

static void dma_clear_isr(dma_t *dma)
{
	switch ((uint32_t)dma->stream)
	{
		case (uint32_t)DMA1_Stream0:
			return DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF0);
		case (uint32_t)DMA1_Stream1:
			return DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF1);
		case (uint32_t)DMA1_Stream2:
			return DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF2);
		case (uint32_t)DMA1_Stream3:
			return DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF3);
		case (uint32_t)DMA1_Stream4:
			return DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF4);
		case (uint32_t)DMA1_Stream5:
			return DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF5);
		case (uint32_t)DMA1_Stream6:
			return DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF6);
		case (uint32_t)DMA1_Stream7:
			return DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF7);

		case (uint32_t)DMA2_Stream0:
			return DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF0);
		case (uint32_t)DMA2_Stream1:
			return DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF1);
		case (uint32_t)DMA2_Stream2:
			return DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF2);
		case (uint32_t)DMA2_Stream3:
			return DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF3);
		case (uint32_t)DMA2_Stream4:
			return DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF4);
		case (uint32_t)DMA2_Stream5:
			return DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF5);
		case (uint32_t)DMA2_Stream6:
			return DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF6);
		case (uint32_t)DMA2_Stream7:
			return DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF7);
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

	DMA_ITConfig(dma->stream, DMA_IT_TC, DISABLE);
	DMA_Cmd(dma->stream, DISABLE);

	req = dma->reqs;
	dma->reqs = NULL; // free the dma before complete so complete can sched another
	if (req->complete != NULL)
		req->complete(req, req->complete_param);
}

void DMA1_Stream0_IRQHandler(void)
{
	dma_t *dma = dma1_irq_list[0];
	if (DMA_GetITStatus(dma->stream, DMA_IT_TCIF0))
	{
		DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF0);
		dma_irq_handler(dma);
	}
}

void DMA1_Stream1_IRQHandler(void)
{
	dma_t *dma = dma1_irq_list[1];
	if (DMA_GetITStatus(dma->stream, DMA_IT_TCIF1))
	{
		DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF1);
		dma_irq_handler(dma);
	}
}

void DMA1_Stream2_IRQHandler(void)
{
	dma_t *dma = dma1_irq_list[2];
	if (DMA_GetITStatus(dma->stream, DMA_IT_TCIF2))
	{
		DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF2);
		dma_irq_handler(dma);
	}
}

void DMA1_Stream3_IRQHandler(void)
{
	dma_t *dma = dma1_irq_list[3];
	if (DMA_GetITStatus(dma->stream, DMA_IT_TCIF3))
	{
		DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF3);
		dma_irq_handler(dma);
	}
}

void DMA1_Stream4_IRQHandler(void)
{
	dma_t *dma = dma1_irq_list[4];
	if (DMA_GetITStatus(dma->stream, DMA_IT_TCIF4))
	{
		DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF4);
		dma_irq_handler(dma);
	}
}

void DMA1_Stream5_IRQHandler(void)
{
	dma_t *dma = dma1_irq_list[5];
	if (DMA_GetITStatus(dma->stream, DMA_IT_TCIF5))
	{
		DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF5);
		dma_irq_handler(dma);
	}
}

void DMA1_Stream6_IRQHandler(void)
{
	dma_t *dma = dma1_irq_list[6];
	if (DMA_GetITStatus(dma->stream, DMA_IT_TCIF6))
	{
		DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF6);
		dma_irq_handler(dma);
	}
}

void DMA1_Stream7_IRQHandler(void)
{
	dma_t *dma = dma1_irq_list[7];
	if (DMA_GetITStatus(dma->stream, DMA_IT_TCIF7))
	{
		DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF7);
		dma_irq_handler(dma);
	}
}

void DMA2_Stream0_IRQHandler(void)
{
	dma_t *dma = dma2_irq_list[0];
	if (DMA_GetITStatus(dma->stream, DMA_IT_TCIF0))
	{
		DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF0);
		dma_irq_handler(dma);
	}
}

void DMA2_Stream1_IRQHandler(void)
{
	dma_t *dma = dma2_irq_list[1];
	if (DMA_GetITStatus(dma->stream, DMA_IT_TCIF1))
	{
		DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF1);
		dma_irq_handler(dma);
	}
}

void DMA2_Stream2_IRQHandler(void)
{
	dma_t *dma = dma2_irq_list[2];
	if (DMA_GetITStatus(dma->stream, DMA_IT_TCIF2))
	{
		DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF2);
		dma_irq_handler(dma);
	}
}

void DMA2_Stream3_IRQHandler(void)
{
	dma_t *dma = dma2_irq_list[3];
	if (DMA_GetITStatus(dma->stream, DMA_IT_TCIF3))
	{
		DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF3);
		dma_irq_handler(dma);
	}
}

void DMA2_Stream4_IRQHandler(void)
{
	dma_t *dma = dma2_irq_list[4];
	if (DMA_GetITStatus(dma->stream, DMA_IT_TCIF4))
	{
		DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF4);
		dma_irq_handler(dma);
	}
}

void DMA2_Stream5_IRQHandler(void)
{
	dma_t *dma = dma2_irq_list[5];
	if (DMA_GetITStatus(dma->stream, DMA_IT_TCIF5))
	{
		DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF5);
		dma_irq_handler(dma);
	}
}

void DMA2_Stream6_IRQHandler(void)
{
	dma_t *dma = dma2_irq_list[6];
	if (DMA_GetITStatus(dma->stream, DMA_IT_TCIF6))
	{
		DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF6);
		dma_irq_handler(dma);
	}
}

void DMA2_Stream7_IRQHandler(void)
{
	dma_t *dma = dma2_irq_list[7];
	if (DMA_GetITStatus(dma->stream, DMA_IT_TCIF7))
	{
		DMA_ClearITPendingBit(dma->stream, DMA_IT_TCIF7);
		dma_irq_handler(dma);
	}
}


static dma_request_t dma_memcpy_req;

static dma_memcpy_complete_event_t dma_memcpy_complete = NULL;

static void memcpy_complete(dma_request_t *req, void *param)
{
	if (req == &dma_memcpy_req && dma_memcpy_complete != NULL)
	{
		void *src = (void *)req->st_dma_init.DMA_PeripheralBaseAddr;
		void *dst = (void *)req->st_dma_init.DMA_Memory0BaseAddr;
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

	req->st_dma_init.DMA_Channel = dma->channel;
	req->st_dma_init.DMA_PeripheralBaseAddr = (uint32_t)src;
	req->st_dma_init.DMA_Memory0BaseAddr = (uint32_t)dst;
	req->st_dma_init.DMA_DIR = DMA_DIR_MemoryToMemory;
	req->st_dma_init.DMA_BufferSize = len;
	req->st_dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	req->st_dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
	req->st_dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	req->st_dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	req->st_dma_init.DMA_Mode = DMA_Mode_Normal;
	req->st_dma_init.DMA_Priority = DMA_Priority_Low;
	req->st_dma_init.DMA_FIFOMode = DMA_FIFOMode_Enable;
	req->st_dma_init.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	req->st_dma_init.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	req->st_dma_init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

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

	dma_clear_isr(dma);
	DMA_Cmd(dma->stream, DISABLE);
	while (DMA_GetCmdStatus(dma->stream))
	{}
	DMA_Init(dma->stream, &req->st_dma_init);
	DMA_ITConfig(dma->stream, DMA_IT_TC, ENABLE);
	DMA_Cmd(dma->stream, ENABLE);
}

int dma_remaining(dma_request_t *req)
{
	dma_t *dma = req->dma;
	return dma->stream->NDTR;
}

void dma_cancel(dma_t *dma)
{
	if (dma == NULL)
		return;

	dma_clear_isr(dma);
	DMA_Cmd(dma->stream, DISABLE);
	while (DMA_GetCmdStatus(dma->stream))
	{}
	dma->reqs = NULL;
}

void dma_init(dma_t *dma)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	if (dma == NULL)
		return;

	dma->reqs = NULL;

	// enable clocks
	switch ((uint32_t)dma->stream)
	{
		case (uint32_t)DMA1_Stream0:
		case (uint32_t)DMA1_Stream2:
		case (uint32_t)DMA1_Stream3:
		case (uint32_t)DMA1_Stream4:
		case (uint32_t)DMA1_Stream5:
		case (uint32_t)DMA1_Stream6:
		case (uint32_t)DMA1_Stream7:
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
			break;
		case (uint32_t)DMA2_Stream0:
		case (uint32_t)DMA2_Stream2:
		case (uint32_t)DMA2_Stream3:
		case (uint32_t)DMA2_Stream4:
		case (uint32_t)DMA2_Stream5:
		case (uint32_t)DMA2_Stream6:
		case (uint32_t)DMA2_Stream7:
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
			break;
		default:
			/// todo error !
			break;
	}

	// enable nvic
	NVIC_InitStructure.NVIC_IRQChannel = dma_irq(dma);
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = dma->preemption_priority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

