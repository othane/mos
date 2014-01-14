/**
 * @file dma.h
 *
 * @brief dma module of hal
 *
 * @author OT
 *
 * @date Jan 2014
 *
 */

#ifndef __DMA__
#define __DMA__

typedef struct dma_t dma_t;


/**
 * @brief do a memcpy using the dma (ie in the background using hw)
 * @param dma dma used to do the work
 * @param dst destination of memcpy
 * @param src source of memcpy
 * @param len number of bytes copied in the memcpy
 * @param complete call this when the memcpy is complete
 */
typedef void (*dma_memcpy_complete_event_t)(dma_t *dma, void *dst, void *src, int len);
void dma_memcpy(dma_t *dma, void *dst, void *src, int len, dma_memcpy_complete_event_t complete);


/**
 * @brief initialise a dma channel
 */
void dma_init(dma_t *dma);

#endif

