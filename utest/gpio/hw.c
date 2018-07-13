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


// default gpio
#if defined STM32F10X_CL

#include <stm32f10x_conf.h>
#include <gpio_hw.h>
gpio_pin_t gpio_in 			= {GPIOA, {GPIO_Pin_15, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};
gpio_pin_t gpio_led0		= {GPIOC, {GPIO_Pin_4,  GPIO_Speed_50MHz, GPIO_Mode_Out_PP}};
gpio_pin_t gpio_led1		= {GPIOC, {GPIO_Pin_4,  GPIO_Speed_50MHz, GPIO_Mode_Out_PP}};
gpio_pin_t gpio_led2		= {GPIOC, {GPIO_Pin_4,  GPIO_Speed_50MHz, GPIO_Mode_Out_PP}};
gpio_pin_t gpio_led3		= {GPIOC, {GPIO_Pin_4,  GPIO_Speed_50MHz, GPIO_Mode_Out_PP}};

#elif defined STM32F37X

#include <stm32f37x_conf.h>
#include <gpio_hw.h>
gpio_pin_t gpio_in 			= {GPIOB, {GPIO_Pin_0, GPIO_Mode_IN, GPIO_Speed_50MHz, 0, GPIO_PuPd_NOPULL}};
gpio_pin_t gpio_led0		= {GPIOC, {GPIO_Pin_0,  GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}};
gpio_pin_t gpio_led1		= {GPIOC, {GPIO_Pin_1,  GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}};
gpio_pin_t gpio_led2		= {GPIOC, {GPIO_Pin_2,  GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}};
gpio_pin_t gpio_led3		= {GPIOC, {GPIO_Pin_3,  GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}};
gpio_pin_t gpio_tri_state	= {GPIOA, {GPIO_Pin_3,  GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}};

#elif defined STM32F40_41xxx

#include <stm32f4xx_conf.h>
#include <gpio_hw.h>
gpio_pin_t gpio_in 			= {GPIOB, {GPIO_Pin_2,  GPIO_Mode_IN, GPIO_Speed_50MHz, 0, GPIO_PuPd_NOPULL}};
gpio_pin_t gpio_led0		= {GPIOC, {GPIO_Pin_0,  GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}};
gpio_pin_t gpio_led1		= {GPIOC, {GPIO_Pin_1,  GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}};
gpio_pin_t gpio_led2		= {GPIOC, {GPIO_Pin_2,  GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}};
gpio_pin_t gpio_led3		= {GPIOC, {GPIO_Pin_3,  GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}};
gpio_pin_t gpio_tri_state	= {GPIOA, {GPIO_Pin_3,  GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}};

#endif
