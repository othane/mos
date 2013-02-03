/**
 * @file spis_hw.h
 * 
 * @brief this contains hw definitions for configuration via hw.c only (it is not a run time interface !)
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */

#ifndef __SPIS_HW__
#define __SPIS_HW__


#include "hal.h"


// internal representation of a slave spi device
struct spis_t
{
	SPI_TypeDef *channel;

	// device pins
	gpio_pin_t *nss, *sck, *miso, *mosi;

	// transaction start
	spis_trans_start_stop trans_start;				///< this is called when nss goes lo starting a new transaction
	spis_trans_start_stop trans_stop;				///< this is called when nss goes hi ending a current transaction
	
	// read buffers
	uint8_t *read_buf;								///< buffer to store the read results in
	int16_t read_buf_len;							///< size of the read buffer
	int16_t read_count;								///< number of bytes actually read from the spi
	spis_read_complete read_complete_cb;			///< call this when number we have read read_buf_len bytes, or the spi transaction ends (nss hi)
	void *read_complete_param;						///< user callback, pass it to read_cb
	
	// write buffers
	uint8_t *write_buf;								///< buffer to transmit to the master
	int16_t write_buf_len;							///< number of bytes to transmit
	int16_t write_count;							///< number of bytes transmitted so far
	spis_write_complete write_complete_cb;			///< call this when we have transmitted write_buf_len bytes, or the spi transaction ends (nss hi)
	void *write_complete_param;						///< user callback, pass it to write_cb
};


#endif


