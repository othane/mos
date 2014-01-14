/**
 * @file dma_utest.c
 *
 * @brief test dma support for memcpy
 *
 * @author OT
 *
 * @date Jan 2014
 *
 */


#include <string.h>
#include <hal.h>

#define WORD uint8_t
#define BUF_LEN 10
static WORD pat0[BUF_LEN] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
static WORD pat1[BUF_LEN] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
static WORD pat_dst[BUF_LEN] = {0,};

void dma_test_complete(dma_t *dma, void *dst, void *src, int len)
{
	if (src == pat0)
		dma_memcpy(&mem_dma, pat_dst, pat1, BUF_LEN, dma_test_complete);
	else if (src == pat1)
		dma_memcpy(&mem_dma, pat_dst, pat0, BUF_LEN, dma_test_complete);
	else
		// error !
		while (1)
		{}
}

void dma_test(void)
{
	dma_init(&mem_dma);
	dma_memcpy(&mem_dma, pat_dst, pat0, BUF_LEN, dma_test_complete);
}


int main(void)
{
	sys_init();
	dma_test();

	// do nothing
	while (1)
	{}

	return 0;
}

