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
#include <gpio_hw.h>
gpio_pin_t gpio_mco 		= {GPIOA, {GPIO_Pin_8,  GPIO_Speed_50MHz, GPIO_Mode_AF_PP}};
gpio_pin_t gpio_systick		= {GPIOA, {GPIO_Pin_10, GPIO_Speed_50MHz, GPIO_Mode_Out_PP}};
#elif defined STM32F37X
#include <stm32f37x_conf.h>
#include <gpio_hw.h>
gpio_pin_t gpio_mco 		= {GPIOA, {GPIO_Pin_8, GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}, 0};
gpio_pin_t gpio_systick		= {GPIOA, {GPIO_Pin_10, GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL}};
#endif

