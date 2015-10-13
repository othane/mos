/**
 * @file hw.c
 * 
 * @brief implements the default stm32f107x hw available
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
#error "pwm not supported on stm32f107x"

#elif defined STM32F37X
#include <stm32f37x_conf.h>
#include <pwm_hw.h>

gpio_pin_t pb6 = {
	.port = GPIOB,
	.cfg = {.GPIO_Pin = GPIO_Pin_6, .GPIO_Mode = GPIO_Mode_AF,
			.GPIO_Speed = GPIO_Speed_50MHz, .GPIO_OType = GPIO_OType_PP, 
			.GPIO_PuPd = GPIO_PuPd_NOPULL},
	.af = 10,
};

tmr_t tmr3 = {
	.tim = TIM3,
	.freq = 20000,
	#ifdef SYNC
	.sync = {
		// master
		.master_slave = TIM_MasterSlaveMode_Enable,
		.slave_mode = 0,
		.output_trigger = TIM_TRGOSource_Enable,
	},
	#endif
};

pwm_channel_t pwm0 = {
	.tmr = &tmr3,
	.pin = &pb6,
	.ch = TIM_Channel_3,
	.duty = 0.5,
	.oc_cfg = { .TIM_OCMode = TIM_OCMode_PWM1, .TIM_OutputState = TIM_OutputState_Enable,
				.TIM_OCPolarity = TIM_OCPolarity_Low}
};

gpio_pin_t pb7 = {
	.port = GPIOB,
	.cfg = {.GPIO_Pin = GPIO_Pin_7, .GPIO_Mode = GPIO_Mode_AF,
			.GPIO_Speed = GPIO_Speed_50MHz, .GPIO_OType = GPIO_OType_PP, 
			.GPIO_PuPd = GPIO_PuPd_NOPULL},
	.af = 2,
};

tmr_t tmr4 = {
	.tim = TIM4,
	.freq = 10000,
	#ifdef SYNC
	.sync = {
		// slave
		.master_slave = TIM_MasterSlaveMode_Enable,
		.slave_mode = TIM_SlaveMode_Gated,
		.output_trigger = TIM_TRGOSource_Update,
		.input_trigger = TIM_TS_ITR2,
	},
	#endif
};

pwm_channel_t pwm1 = {
	.tmr = &tmr4,
	.pin = &pb7,
	.ch = TIM_Channel_2,
	.duty = 0.5,
	.oc_cfg = { .TIM_OCMode = TIM_OCMode_PWM1, .TIM_OutputState = TIM_OutputState_Enable,
				.TIM_OCPolarity = TIM_OCPolarity_Low}
};

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
	.freq = 1000,
};

pwm_channel_t pwm2 = {
	.tmr = &tmr19,
	.pin = &pc11,
	.ch = TIM_Channel_2,
	.duty = 0.25,
	.oc_cfg = { .TIM_OCMode = TIM_OCMode_PWM1, .TIM_OutputState = TIM_OutputState_Enable,
				.TIM_OCPolarity = TIM_OCPolarity_Low}
};

pwm_channel_t pwm3 = {
	.tmr = &tmr19,
	.pin = &pc12,
	.ch = TIM_Channel_3,
	.duty = 0.75,
	.oc_cfg = { .TIM_OCMode = TIM_OCMode_PWM1, .TIM_OutputState = TIM_OutputState_Enable,
				.TIM_OCPolarity = TIM_OCPolarity_Low}
};

#endif
