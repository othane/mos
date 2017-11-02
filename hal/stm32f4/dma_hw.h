/**
 * @file dma_hw.h
 *
 * @brief this contains hw definitions for the dma configuration
 *
 * @author OT
 *
 * @date Jan 2014
 *
 */

#ifndef __DMA_HW__
#define __DMA_HW__

#include <hal.h>

typedef struct dma_request_t dma_request_t;

typedef void (*dma_complete_event_t)(dma_request_t *req, void *param);

struct dma_request_t
{
	DMA_InitTypeDef  st_dma_init;
	dma_complete_event_t complete;
	void *complete_param;
	struct dma_t *dma;
};

void dma_request(dma_request_t *req);

int dma_remaining(dma_request_t *req);

struct dma_t
{
	DMA_Stream_TypeDef *stream;
	uint32_t channel;
	struct dma_request_t *reqs;
	uint8_t preemption_priority;
	uint32_t isr_status;
	uint8_t circ;
};

#endif

