/**
 * @file hw.c
 *
 * @brief spis hw file for the stm32f1
 *
 * @see hw.h for instructions to override the defaults
 *
 * @author OT
 *
 * @date Feb 2017
 *
 */

#include <hal.h>

#if defined STM32F37X

	#include <stm32f37x_conf.h>
	#include <tmr_hw.h>
	tmr_t tmr_dev =
	{
		.tim = TIM3,
		.freq = 1,
		.stop_on_halt = 1,
	};

#elif defined STM32F40_41xxx

	#include <stm32f4xx_conf.h>
	#include <tmr_hw.h>
	tmr_t tmr_dev =
	{
		.tim = TIM3,
		.freq = 1,
		.stop_on_halt = 1,
	};

#else

	#error "tmr not supported on unknown target"

#endif
