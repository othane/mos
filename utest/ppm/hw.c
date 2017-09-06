/**
 * @file hw.c
 * 
 * @brief hw setup for ppm unit test
 *
 * @see hw.h for instructions to override the defaults
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */

#include <hal.h>


#if defined STM32F10X_CL

	#include <stm32f10x_conf.h>
	#error "ppm not supported on stm32f107x"

#elif defined STM32F37X

	#include <stm32f37x_conf.h>
	#include <ppm_hw.h>

	gpio_pin_t pc12 = 
	{
		.port = GPIOC,
		.cfg = {.GPIO_Pin = GPIO_Pin_12, .GPIO_Mode = GPIO_Mode_AF,
				.GPIO_Speed = GPIO_Speed_50MHz, .GPIO_OType = GPIO_OType_PP, 
				.GPIO_PuPd = GPIO_PuPd_NOPULL},
		.af = 2,
	};

	gpio_pin_t pc11 = 
	{
		.port = GPIOC,
		.cfg = {.GPIO_Pin = GPIO_Pin_11, .GPIO_Mode = GPIO_Mode_AF,
				.GPIO_Speed = GPIO_Speed_50MHz, .GPIO_OType = GPIO_OType_PP, 
				.GPIO_PuPd = GPIO_PuPd_NOPULL},
		.af = 2,
	};

	tmr_t tmr19 = {
		.tim = TIM19,
	};

	ppm_channel_t ppm_ref = {
		.tmr = &tmr19,
		.pin = &pc11,
		.ch = TIM_Channel_2,
		.phs = 0.0,
		.freq = 1000,
		.oc_cfg = { .TIM_OCMode = TIM_OCMode_Toggle, .TIM_OutputState = TIM_OutputState_Enable}
	};

	ppm_channel_t ppm_drift = {
		.tmr = &tmr19,
		.pin = &pc12,
		.ch = TIM_Channel_3,
		.phs = 1.0,
		.freq = 1000,
		.oc_cfg = { .TIM_OCMode = TIM_OCMode_Toggle, .TIM_OutputState = TIM_OutputState_Enable}
	};

#elif defined STM32F40_41xxx

	#include <stm32f4xx_conf.h>
	#include <ppm_hw.h>

	gpio_pin_t pb10 =
	{
		.port = GPIOB,
		.cfg = {.GPIO_Pin = GPIO_Pin_10, .GPIO_Mode = GPIO_Mode_AF,
				.GPIO_Speed = GPIO_Speed_50MHz, .GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd = GPIO_PuPd_NOPULL},
		.af = 1,
	};

	gpio_pin_t pb11 =
	{
		.port = GPIOB,
		.cfg = {.GPIO_Pin = GPIO_Pin_11, .GPIO_Mode = GPIO_Mode_AF,
				.GPIO_Speed = GPIO_Speed_50MHz, .GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd = GPIO_PuPd_NOPULL},
		.af = 1,
	};

	tmr_t tmr2 = {
		.tim = TIM2,
	};

	ppm_channel_t ppm_ref = {
		.tmr = &tmr2,
		.pin = &pb10,
		.ch = TIM_Channel_3,
		.phs = 0.0,
		.freq = 1000,
		.oc_cfg = { .TIM_OCMode = TIM_OCMode_Toggle, .TIM_OutputState = TIM_OutputState_Enable}
	};

	ppm_channel_t ppm_drift = {
		.tmr = &tmr2,
		.pin = &pb11,
		.ch = TIM_Channel_4,
		.phs = 1.0,
		.freq = 1000,
		.oc_cfg = { .TIM_OCMode = TIM_OCMode_Toggle, .TIM_OutputState = TIM_OutputState_Enable}
	};

#else

	#error "pwm not supported on unknown target"

#endif
