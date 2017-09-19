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
	gpio_pin_t gpio_spi1_sck    = {GPIOA, {GPIO_Pin_5,  GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_DOWN}, 5};
	gpio_pin_t gpio_spi1_miso   = {GPIOA, {GPIO_Pin_6,  GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_DOWN}, 5};
	gpio_pin_t gpio_spi1_mosi   = {GPIOA, {GPIO_Pin_7,  GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_DOWN}, 5};
	gpio_pin_t gpio_spi1_nss    = {GPIOA, {GPIO_Pin_4,  GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP,}, 0};
	
	gpio_pin_t gpio_spi2_sck    = {GPIOA, {GPIO_Pin_8,  GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_DOWN}, 5};
	gpio_pin_t gpio_spi2_miso   = {GPIOA, {GPIO_Pin_9,  GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_DOWN}, 5};
	gpio_pin_t gpio_spi2_mosi   = {GPIOA, {GPIO_Pin_10, GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_DOWN}, 5};
	gpio_pin_t gpio_spi2_nss    = {GPIOA, {GPIO_Pin_11, GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_DOWN}, 5};
	
	gpio_pin_t gpio_spi3_sck    = {GPIOB, {GPIO_Pin_3,  GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_DOWN}, 6};
	gpio_pin_t gpio_spi3_miso   = {GPIOB, {GPIO_Pin_4,  GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_DOWN}, 6};
	gpio_pin_t gpio_spi3_mosi   = {GPIOB, {GPIO_Pin_5,  GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_DOWN}, 6};
	gpio_pin_t gpio_spi3_nss    = {GPIOA, {GPIO_Pin_15, GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_DOWN}, 6};

	#include <dma_hw.h>
	dma_t spis_rx_dma =
	{
		.channel = DMA1_Channel4,
	};
	dma_t spis_tx_dma =
	{
		.channel = DMA1_Channel5,
	};
	dma_t spim_rx_dma =
	{
		.channel = DMA1_Channel2,
	};
	dma_t spim_tx_dma =
	{
		.channel = DMA1_Channel3,
	};

	#include <spis_hw.h>
	spis_t spis_dev =
	{
		.channel = SPI2,   // channel
		.st_spi_init = {SPI_Direction_2Lines_FullDuplex, SPI_Mode_Slave,
						SPI_DataSize_8b, SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_NSS_Hard,
						SPI_BaudRatePrescaler_256, SPI_FirstBit_MSB, 1},
		.nss = &gpio_spi2_nss, // select line gpio
		.sck = &gpio_spi2_sck, // clk line gpio
		.miso = &gpio_spi2_miso, // miso gpio
		.mosi = &gpio_spi2_mosi, // mosi gpio

		.rx_dma = &spis_rx_dma,
		.tx_dma = &spis_tx_dma,
	};
	
	#include <spim_hw.h>
	spim_xfer_opts spim_dev_opts =
	{
		.speed = 0,
		.st_opts = {SPI_Direction_2Lines_FullDuplex, SPI_Mode_Master, 
					SPI_DataSize_8b, SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_NSS_Soft, 
					SPI_BaudRatePrescaler_256, SPI_FirstBit_MSB, 1},
	};

	gpio_pin_t *spim_dev_nss[] = {&gpio_spi1_nss, NULL};
	spim_t spim_dev =
	{
		.channel = SPI1,   // channel
		.idle_address = 0x00, // bus should go to this state when idle
		.nss = spim_dev_nss, // nss/address lines
		.sck = &gpio_spi1_sck, // clk line gpio
		.miso = &gpio_spi1_miso, // miso gpio
		.mosi = &gpio_spi1_mosi, // mosi gpio

		.rx_dma = &spim_rx_dma,
		.tx_dma = &spim_tx_dma,
	};

#elif defined STM32F40_41xxx

	#include <stm32f4xx_conf.h>
	#include <gpio_hw.h>
	gpio_pin_t gpio_spi1_nss    = {GPIOA, {GPIO_Pin_4, GPIO_Mode_AF, GPIO_Speed_50MHz}, 5};
	gpio_pin_t gpio_spi1_sck    = {GPIOA, {GPIO_Pin_5, GPIO_Mode_AF, GPIO_Speed_50MHz}, 5};
	gpio_pin_t gpio_spi1_miso   = {GPIOA, {GPIO_Pin_6, GPIO_Mode_AF, GPIO_Speed_50MHz}, 5};
	gpio_pin_t gpio_spi1_mosi   = {GPIOA, {GPIO_Pin_7, GPIO_Mode_AF, GPIO_Speed_50MHz}, 5};

	gpio_pin_t gpio_spi2_nss    = {GPIOB, {GPIO_Pin_12, GPIO_Mode_OUT, GPIO_Speed_50MHz}, 5};
	gpio_pin_t gpio_spi2_sck    = {GPIOB, {GPIO_Pin_10, GPIO_Mode_AF, GPIO_Speed_50MHz}, 5};
	gpio_pin_t gpio_spi2_miso   = {GPIOB, {GPIO_Pin_14, GPIO_Mode_AF, GPIO_Speed_50MHz}, 5};
	gpio_pin_t gpio_spi2_mosi   = {GPIOB, {GPIO_Pin_15, GPIO_Mode_AF, GPIO_Speed_50MHz}, 5};

	#include <dma_hw.h>
	dma_t spis_rx_dma =
	{
		.stream = DMA2_Stream0,
		.channel = DMA_Channel_3,
	};
	dma_t spis_tx_dma =
	{
		.stream = DMA2_Stream3,
		.channel = DMA_Channel_3,
	};

	dma_t spim_rx_dma =
	{
		.stream = DMA1_Stream3,
		.channel = DMA_Channel_0,
	};
	dma_t spim_tx_dma =
	{
		.stream = DMA1_Stream4,
		.channel = DMA_Channel_0,
	};

	#include <spis_hw.h>
	spis_t spis_dev =
	{
		.channel = SPI1,   // channel
		.st_spi_init = {SPI_Direction_2Lines_FullDuplex, SPI_Mode_Slave,
						SPI_DataSize_8b, SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_NSS_Hard,
						SPI_BaudRatePrescaler_2, SPI_FirstBit_MSB, 1},
		.nss = &gpio_spi1_nss, // select line gpio
		.sck = &gpio_spi1_sck, // clk line gpio
		.miso = &gpio_spi1_miso, // miso gpio
		.mosi = &gpio_spi1_mosi, // mosi gpio

		.rx_dma = &spis_rx_dma,
		.tx_dma = &spis_tx_dma,
	};

	#include <spim_hw.h>
	spim_xfer_opts spim_dev_opts =
	{
		.speed = 0,
		.st_opts = {SPI_Direction_2Lines_FullDuplex, SPI_Mode_Master,
					SPI_DataSize_8b, SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_NSS_Soft,
					SPI_BaudRatePrescaler_256, SPI_FirstBit_MSB, 1},
	};

	gpio_pin_t *spim_dev_nss[] = {&gpio_spi2_nss, NULL};
	spim_t spim_dev =
	{
		.channel = SPI2,   // channel
		.idle_address = 0x00, // bus should go to this state when idle
		.nss = spim_dev_nss, // nss/address lines
		.sck = &gpio_spi2_sck, // clk line gpio
		.miso = &gpio_spi2_miso, // miso gpio
		.mosi = &gpio_spi2_mosi, // mosi gpio

		.rx_dma = &spim_rx_dma,
		.tx_dma = &spim_tx_dma,
	};

#endif
