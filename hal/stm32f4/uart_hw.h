/**
 * @file uart_hw.h
 *
 * @brief this contains hw definitions for configuration via hw.c only (it is not a run time interface !)
 *
 * @author OT
 *
 * @date Feb 2017
 *
 */

#ifndef __UART_HW__
#define __UART_HW__


#include "hal.h"
#include "dma_hw.h"


// internal representation of a slave uart device
struct uart_t
{
	USART_TypeDef *channel;					 	///< uart channel, ie USART1..USART3, 
	gpio_pin_t *rx, *tx;						///< uart pins
	USART_InitTypeDef cfg;						///< uart config (baudrate etc)
	uint8_t preemption_priority;				///< set the pre-emption priority for uart interrupts

	// read buffers
	void *read_buf;								///< buffer to store the read results in
	int16_t read_buf_len;						///< size of the read buffer
	int16_t read_count;							///< number of bytes actually read from the uart 
	uart_read_complete_cb read_complete_cb;		///< call this when the read completes
	void *read_complete_param;					///< user callback, pass it to read_cb
	dma_t *rx_dma;								///< optional dma used for rx (ie dont use isr, do it in hw)
	dma_request_t rx_dma_req;					///< used by rx_dma

	// write buffers
	void *write_buf;							///< buffer to transmit
	int16_t write_buf_len;						///< number of bytes to transmit
	int16_t write_count;						///< number of bytes transmitted so far
	uart_write_complete_cb write_complete_cb;	///< call this when we have transmitted write_buf_len bytes
	void *write_complete_param;					///< user callback param passed to write_complete_cb
	dma_t *tx_dma;								///< optional dma used for tx (ie dont use isr, do it in hw)
	dma_request_t tx_dma_req;					///< used by tx_dma
};

#endif
