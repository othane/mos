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


// internal representation of a master spi device
struct spim_t
{
	SPI_TypeDef *channel;
	SPI_InitTypeDef st_spi_init;					///< details of how the spim should run (not all options are supported yet)

	// device pins
	gpio_pin_t *nss, *sck, *miso, *mosi;
	gpio_pin_t **addr;

	// transfer
	uint8_t *read_buf;                              ///< buffer to store the slave data
	int16_t read_count;                             ///< number of bytes received so far
	uint8_t *write_buf;                             ///< buffer to transmit to the slave
	int16_t write_count;                            ///< number of bytes transmitted so far
	int16_t len;                           			///< size of the read/write buffers
	spim_xfer_complete xfer_complete;				///< called once len bytes are transfered
	void *xfer_complete_param;						///< completion parameter
	dma_t *rx_dma;									///< optional dma used for rx (ie dont use isr, do it in hw)
	dma_request_t rx_dma_req;						///< used by rx_dma
	dma_t *tx_dma;									///< optional dma used for tx (ie dont use isr, do it in hw)
	dma_request_t tx_dma_req;						///< used by tx_dma
};


#endif
