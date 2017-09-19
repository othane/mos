/**
 * @file hw.c
 *
 * @brief dma hw file
 *
 * @see hw.h for instructions to override the defaults
 *
 * @author OT
 *
 * @date March 2013
 *
 */

#include <hal.h>

#if defined STM32F37X

	#include <stm32f37x_conf.h>
	#include <dma_hw.h>
	dma_t mem_dma =
	{
		.channel = DMA1_Channel1,
	};

#elif defined STM32F40_41xxx

	#include <stm32f4xx_conf.h>
	#include <dma_hw.h>
	dma_t mem_dma =
	{
		.stream = DMA2_Stream0,
		.channel = DMA_Channel_0,
	};

#else

	#warn "no dma available on this arch yet"

#endif
