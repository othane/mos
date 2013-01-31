/**
 * @file hw.c
 * 
 * @brief implements the default stm32f107x hw available
 *
 * @see hw.h for instructions to override the deffaults
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */


#include <hal.h>


// default gpio
#include <gpio_hw.h>
gpio_pin_t gpio_mco 		= {RCC_APB2Periph_GPIOA, GPIOA, {GPIO_Pin_8,  GPIO_Speed_50MHz, GPIO_Mode_AF_PP}};
gpio_pin_t gpio_pa10 		= {RCC_APB2Periph_GPIOA, GPIOA, {GPIO_Pin_10, GPIO_Speed_50MHz, GPIO_Mode_Out_PP}};


