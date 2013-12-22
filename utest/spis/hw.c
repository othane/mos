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

#include <hal.h>

#if defined STM32F10X_CL
	#include <stm32f10x_conf.h>
	#include <gpio_hw.h>
	gpio_pin_t gpio_spi3_nss    = {GPIOA, {GPIO_Pin_15, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};
	gpio_pin_t gpio_spi3_sck    = {GPIOB, {GPIO_Pin_3,  GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};
	gpio_pin_t gpio_spi3_mosi   = {GPIOB, {GPIO_Pin_5,  GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING}};
	gpio_pin_t gpio_spi3_miso   = {GPIOB, {GPIO_Pin_4,  GPIO_Speed_50MHz, GPIO_Mode_AF_PP}};

	#include <spis_hw.h>
	spis_t spis_dev =
	{
		SPI3,   // channel
		&gpio_spi3_nss, &gpio_spi3_sck, &gpio_spi3_miso, &gpio_spi3_mosi, // pins
		0,
	};
#elif defined STM32F37X
	#include <stm32f37x_conf.h>
	#include <gpio_hw.h>
	gpio_pin_t gpio_spi3_nss    = {GPIOA, {GPIO_Pin_15, GPIO_Mode_AF, GPIO_Speed_50MHz}, 6};
	gpio_pin_t gpio_spi3_sck    = {GPIOB, {GPIO_Pin_3,  GPIO_Mode_AF, GPIO_Speed_50MHz}, 6};
	gpio_pin_t gpio_spi3_mosi   = {GPIOB, {GPIO_Pin_5,  GPIO_Mode_AF, GPIO_Speed_50MHz}, 6};
	gpio_pin_t gpio_spi3_miso   = {GPIOB, {GPIO_Pin_4,  GPIO_Mode_AF, GPIO_Speed_50MHz}, 6};

	#include <spis_hw.h>
	spis_t spis_dev =
	{
		SPI3,   // channel
		{SPI_Direction_2Lines_FullDuplex, SPI_Mode_Slave, SPI_DataSize_8b, SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_NSS_Hard, SPI_BaudRatePrescaler_2, SPI_FirstBit_MSB, 0},
		&gpio_spi3_nss, &gpio_spi3_sck, &gpio_spi3_miso, &gpio_spi3_mosi, // pins
		0,
	};
#endif
