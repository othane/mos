/**
 * @file i2cm_hw.h
 *
 * @brief this contains hw definitions for configuration via hw.c only (it is not a run time interface !)
 *
 * @author OT
 *
 * @date Jan 2017
 *
 */

#ifndef __I2CM_HW__
#define __I2CM_HW__


#include "hal.h"
#include "dma_hw.h"

struct i2cm_xfer_opts
{
	uint32_t speed; 			///< go equal to or slower than this, or if 0 use st_opts.SPI_BaudRatePrescaler
	I2C_InitTypeDef st_opts;	///< i2c master setup
};

// internal representation of a master i2c device
struct i2cm_t
{
	I2C_TypeDef *channel;
	uint8_t preemption_priority;
	uint32_t analog_filter;				///< I2C_AnalogFilter_Enable|Disable
	uint32_t digital_filter;			///< 0 - 15 see ??
	uint32_t address_size;				///< I2C_AcknowledgedAddress_7|10bit mode select (maybe this should be on a transfer obj like spi)
	uint32_t timing;					///< required timing (maybe this should be on a transfer obj like spi)

	// device pins
	gpio_pin_t *sda, *scl;							///< standard i2c lines

	// transfer stuff
	uint16_t addr;								///< address to direct the transfer too
	uint8_t *read_buf;							///< buffer to store the slave data
	int16_t read_count;							///< number of bytes received so far
	uint8_t *write_buf;							///< buffer to transmit to the slave
	int16_t write_count;						///< number of bytes transmitted so far
	int16_t len;								///< size of the read/write buffers
	spim_xfer_complete xfer_complete;			///< called once len bytes are transfered
	void *xfer_complete_param;					///< completion parameter
	dma_t *rx_dma;								///< optional dma used for rx (ie dont use isr, do it in hw)
	dma_request_t rx_dma_req;					///< used by rx_dma
	dma_t *tx_dma;								///< optional dma used for tx (ie dont use isr, do it in hw)
	dma_request_t tx_dma_req;					///< used by tx_dma
};


#endif


