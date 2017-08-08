/**
 * @file i2c_hw.h
 *
 * @brief this contains hw definitions for configuration via hw.c only (it is not a run time interface !)
 *
 * @author CC
 *
 * @date Aug 2017
 *
 */

#ifndef __I2C_HW__
#define __I2C_HW__

#include "hal.h"
#include "dma_hw.h"

// internal representation of a slave i2c device
struct i2c_t
{
    I2C_TypeDef *channel;                       ///< i2c channel , ie, I2C1, I2C2
    I2C_InitTypeDef cfg;                        ///< i2c config
    gpio_pin_t *scl,*sda;                       ///< i2c pin

    // read buffers
    void *read_buf;                             ///< buffer to store the read results in                             
    int16_t read_buf_len;                       ///< size of read buffer
    int16_t read_count;                         ///< number of bytes actually read from the uart
    i2c_read_complete_cb read_complete_cb;      ///< call this when the read completes
    void *read_complete_param;                  ///< user callback, pass it to read_cb
    dma_t *rx_dma;                              ///< optional dma used for TX for rx
    dma_request_t rx_dma_req;                   ///< used by rx_dma

    // write buffers
    void *write_buf;                            ///< buffer to transmit
    int16_t write_buf_len;                      ///< number of bytes to transmit
    int16_t write_count;                        ///< number of bytes transmitted so far
    i2c_write_complete_cb write_complete_cb;    ///< call this when we have transmitted write_buf_len bytes
    void *write_complete_param;                 ///< user callback param passed to write_complete_cb
    dma_t *tx_dma;                              ///< optional dma used for tx (ie dont use isr, do it in hw)
    dma_request_t tx_dma_req;                   ///< used by tx_dma    
};

#endif
