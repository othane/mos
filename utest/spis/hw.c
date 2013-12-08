/**
 * @file hw.c
 *
 * @brief spis hw file for the stm32f1
 *
 * @see hw.h for instructions to override the defaults
 *
 * @author OT
 *
 * @date March 2013
 *
 */


#include <stm32f10x_conf.h>
#include <hal.h>


// default gpio
#include <gpio_hw.h>
gpio_pin_t gpio_spi3_nss    = {GPIOA, {GPIO_Pin_15, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};
gpio_pin_t gpio_spi3_sck    = {GPIOB, {GPIO_Pin_3,  GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};
gpio_pin_t gpio_spi3_mosi   = {GPIOB, {GPIO_Pin_5,  GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};
gpio_pin_t gpio_spi3_miso   = {GPIOB, {GPIO_Pin_4,  GPIO_Speed_50MHz, GPIO_Mode_AF_PP}};


// default spi
#include <spis_hw.h>
spis_t spis3 =
{
	SPI3,   // channel
	&gpio_spi3_nss, &gpio_spi3_sck, &gpio_spi3_miso, &gpio_spi3_mosi, // pins
	NULL, NULL, // start stop event
	NULL, 0, 0, NULL, NULL, // read bufs and flags
	NULL, 0, 0, NULL, NULL  // write bufs and flags
};


