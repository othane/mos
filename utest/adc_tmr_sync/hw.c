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
#error "adc not supported on stm32f107x"

#elif defined STM32F37X
#include <stm32f37x_conf.h>
#include <gpio_hw.h>
#include <adc_hw.h>

gpio_pin_t pe12 = 
{
	.port = GPIOE,
	.cfg = {.GPIO_Pin = GPIO_Pin_12, .GPIO_Mode = GPIO_Mode_AN, 
			.GPIO_PuPd = GPIO_PuPd_NOPULL},
};


gpio_pin_t pe11 = 
{
	.port = GPIOE,
	.cfg = {.GPIO_Pin = GPIO_Pin_11, .GPIO_Mode = GPIO_Mode_AN, 
			.GPIO_PuPd = GPIO_PuPd_NOPULL},
};


gpio_pin_t pe10 = 
{
	.port = GPIOE,
	.cfg = {.GPIO_Pin = GPIO_Pin_10, .GPIO_Mode = GPIO_Mode_AN, 
			.GPIO_PuPd = GPIO_PuPd_NOPULL},
};


gpio_pin_t pe9 = 
{
	.port = GPIOE,
	.cfg = {.GPIO_Pin = GPIO_Pin_9, .GPIO_Mode = GPIO_Mode_AN, 
			.GPIO_PuPd = GPIO_PuPd_NOPULL},
};


dma_t adc1_dma =
{
	.channel = DMA2_Channel3,
};


adc_t adc1 =
{
	.base = SDADC1,
	.ref = SDADC_VREF_Ext,
	.SDADC_AINStructure = 
	{
		{.SDADC_InputMode = SDADC_InputMode_SEZeroReference, .SDADC_Gain = SDADC_Gain_1,
			.SDADC_CommonMode = SDADC_CommonMode_VSSA},
		{.SDADC_InputMode = SDADC_InputMode_SEZeroReference, .SDADC_Gain = SDADC_Gain_1,
			.SDADC_CommonMode = SDADC_CommonMode_VSSA},
		{.SDADC_InputMode = SDADC_InputMode_SEZeroReference, .SDADC_Gain = SDADC_Gain_1,
			.SDADC_CommonMode = SDADC_CommonMode_VSSA},
	},
	.dma = &adc1_dma,
	.trigger = 
	{
		.type = SDADC_ExternalTrigInjecConvEdge_Rising,
		.source = SDADC_ExternalTrigInjecConv_T3_CC1,
		.cont = 0,
	}
};


adc_channel_t adc_chanA =
{
	.adc = &adc1,
	.number = SDADC_Channel_0,
	.conf = SDADC_Conf_0,
	.pin = &pe12,
};


adc_channel_t adc_chanB =
{
	.adc = &adc1,
	.number = SDADC_Channel_1,
	.conf = SDADC_Conf_0,
	.pin = &pe11,
};


adc_channel_t adc_chanC =
{
	.adc = &adc1,
	.number = SDADC_Channel_2,
	.conf = SDADC_Conf_0,
	.pin = &pe10,
};


adc_channel_t adc_chanD =
{
	.adc = &adc1,
	.number = SDADC_Channel_7,
	.conf = SDADC_Conf_0,
	.pin = &pe9,
};


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
	.freq = 100,
	.sync = {
		// master
		.master_slave = TIM_MasterSlaveMode_Enable,
		.slave_mode = 0,
		.output_trigger = TIM_TRGOSource_Enable,
	},
};

pwm_channel_t pwm0 = {
	.tmr = &tmr3,
	.pin = &pb6,
	.ch = TIM_Channel_3,
	.duty = 0.5,
	.oc_cfg = { .TIM_OCMode = TIM_OCMode_PWM1, .TIM_OutputState = TIM_OutputState_Enable,
				.TIM_OCPolarity = TIM_OCPolarity_High}
};

pwm_channel_t pwm1 = {
	.tmr = &tmr3,
	.pin = NULL,
	.ch = TIM_Channel_1,
	.duty = 0.01,
	.oc_cfg = { .TIM_OCMode = TIM_OCMode_PWM1, .TIM_OutputState = TIM_OutputState_Enable,
				.TIM_OCPolarity = TIM_OCPolarity_High}
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
	.freq = 50,
	.sync = {
		// slave
		.master_slave = TIM_MasterSlaveMode_Enable,
		.slave_mode = TIM_SlaveMode_Gated,
		.output_trigger = TIM_TRGOSource_Update,
		.input_trigger = TIM_TS_ITR2,
	},
};

pwm_channel_t pwm2 = {
	.tmr = &tmr4,
	.pin = &pb7,
	.ch = TIM_Channel_2,
	.duty = 0.5,
	.oc_cfg = { .TIM_OCMode = TIM_OCMode_PWM1, .TIM_OutputState = TIM_OutputState_Enable,
				.TIM_OCPolarity = TIM_OCPolarity_High}
};

#endif
