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

#if defined STM32F10X_CL
	#warn "no dma available on this arch yet"
#elif defined STM32F37X
	#include <stm32f37x_conf.h>
	#include <dma_hw.h>
	dma_t mem_dma =
	{
		.channel = DMA1_Channel1,
	};
#endif
