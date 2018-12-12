/**
 * @file spim_hw.h
 *
 * @brief this contains hw definitions for configuration via hw.c only (it is not a run time interface !)
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */

#ifndef __SPIM_HW__
#define __SPIM_HW__


#include "hal.h"
#include "dma_hw.h"

struct spim_xfer_opts
{
	uint32_t speed; 			///< go equal to or slower than this, or if 0 use st_opts.SPI_BaudRatePrescaler
	SPI_InitTypeDef st_opts; 	///< spi setup for a particular transaction
};

// internal representation of a master spi device
struct spim_t
{
	SPI_TypeDef *channel;
	uint16_t idle_address;							///< address to select when the bus is idle
	uint8_t preemption_priority;

	// device pins
	gpio_pin_t **nss;								///< null terminated array of address pin where the first item is the LSB and the last is the MSB in the address .. to use HW controlled NSS set st_opts.SPI_NSS = SPI_NSS_Hard and this to NULL
	gpio_pin_t *sck, *miso, *mosi;					///< other standard spi lines

	// transfer
	uint16_t addr;									///< address to direct the transfer too
	uint8_t *read_buf;                              ///< buffer to store the slave data
	int16_t read_count;                             ///< number of bytes received so far
	uint8_t *write_buf;                             ///< buffer to transmit to the slave
	int16_t write_count;                            ///< number of bytes transmitted so far
	int16_t len;                           			///< size of the read/write buffers
	spim_xfer_complete xfer_complete;				///< called once len bytes are transfered
	void *xfer_complete_param;						///< completion parameter
	dma_t *rx_dma;									///< optional dma used for rx (ie dont use isr, do it in hw)
	dma_request_t rx_dma_req;						///< used by rx_dma
	bool rx_completed;								///< true if the rx dma complete interrupt has been called
	dma_t *tx_dma;									///< optional dma used for tx (ie dont use isr, do it in hw)
	dma_request_t tx_dma_req;						///< used by tx_dma
	bool tx_completed;								///< true if the tx dma complete interrupt has been called
};


#endif
