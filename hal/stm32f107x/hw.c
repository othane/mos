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


#include <stm32f10x_conf.h>
#include <hal.h>


// default gpio
#include <gpio_hw.h>
gpio_pin_t gpio_mco 		= {GPIOA, {GPIO_Pin_8,  GPIO_Speed_50MHz, GPIO_Mode_AF_PP}};
gpio_pin_t gpio_pa10 		= {GPIOA, {GPIO_Pin_10, GPIO_Speed_50MHz, GPIO_Mode_Out_PP}};
gpio_pin_t gpio_pa15 		= {GPIOA, {GPIO_Pin_15, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};

gpio_pin_t gpio_spi1_nss 	= {GPIOA, {GPIO_Pin_5, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};
gpio_pin_t gpio_spi1_sck 	= {GPIOA, {GPIO_Pin_7, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};
gpio_pin_t gpio_spi1_mosi	= {GPIOA, {GPIO_Pin_4, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};
gpio_pin_t gpio_spi1_miso	= {GPIOA, {GPIO_Pin_6, GPIO_Speed_50MHz, GPIO_Mode_AF_PP}};

gpio_pin_t gpio_spi2_nss 	= {GPIOB, {GPIO_Pin_12, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};
gpio_pin_t gpio_spi2_sck 	= {GPIOB, {GPIO_Pin_13, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};
gpio_pin_t gpio_spi2_mosi	= {GPIOB, {GPIO_Pin_15, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};
gpio_pin_t gpio_spi2_miso	= {GPIOB, {GPIO_Pin_14, GPIO_Speed_50MHz, GPIO_Mode_AF_PP}};

gpio_pin_t gpio_spi3_nss 	= {GPIOA, {GPIO_Pin_15, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};
gpio_pin_t gpio_spi3_sck 	= {GPIOB, {GPIO_Pin_3,  GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};
gpio_pin_t gpio_spi3_mosi	= {GPIOB, {GPIO_Pin_5,  GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};
gpio_pin_t gpio_spi3_miso	= {GPIOB, {GPIO_Pin_4,  GPIO_Speed_50MHz, GPIO_Mode_AF_PP}};


// default spi 
#include <spis_hw.h>
spis_t spis3 = 
{
	SPI3, 	// channel
	&gpio_spi3_nss, &gpio_spi3_sck, &gpio_spi3_miso, &gpio_spi3_mosi, // pins
	NULL, NULL, // start stop event
	NULL, 0, 0, NULL, NULL, // read bufs and flags
	NULL, 0, 0, NULL, NULL  // write bufs and flags
};


