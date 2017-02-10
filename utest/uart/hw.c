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

#if defined STM32F37X
	#include <stm32f37x_conf.h>
	#include <gpio_hw.h>
	gpio_pin_t gpio_rx = {GPIOD, {GPIO_Pin_6,  GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_UP}, 7};
	gpio_pin_t gpio_tx = {GPIOD, {GPIO_Pin_5,  GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_UP}, 7};

	#include <dma_hw.h>
	dma_t uart_rx_dma =
	{
		.channel = DMA1_Channel6,
	};
	dma_t uart_tx_dma =
	{
		.channel = DMA1_Channel7,
	};

	#include <uart_hw.h>
	uart_t uart_dev =
	{
		.channel = USART2,

		.rx = &gpio_rx,
		.tx = &gpio_tx,

		.cfg = {
			.USART_BaudRate = 115200,
			.USART_WordLength = USART_WordLength_8b,
			.USART_StopBits = USART_StopBits_1,
			.USART_Parity = USART_Parity_No,
			.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
		},

		.rx_dma = &uart_rx_dma,
		.tx_dma = &uart_tx_dma,
	};
#endif
