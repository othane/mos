/**
 * @file hw.c
 * 
 * @brief simple hw for sched (nothing but a debug pin for timing measurements)
 *
 * @author OT
 *
 * @date July 2014
 *
 */

#include <hal.h>

#if defined STM32F10X_CL
	#include <stm32f10x_conf.h>
	#include <gpio_hw.h>
	gpio_pin_t tp1 = {GPIOA, {GPIO_Pin_10, GPIO_Speed_50MHz, GPIO_Mode_Out_PP}};
#elif defined STM32F37X
	#include <stm32f37x_conf.h>
	#include <gpio_hw.h>
	gpio_pin_t tp1 = {GPIOA, {GPIO_Pin_10, GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}};
#endif

