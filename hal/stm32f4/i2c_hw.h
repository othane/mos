/**
 * @file i2c_hw.h
 *
 * @brief I2C Master and SLave interface. This contains hw definitions for
 * configuration via hw.c only (it is not a run time interface !)
 *
 * @author Charles Oram
 *
 * @date May 2018
 *
 */

#ifndef __I2C_HW__
#define __I2C_HW__

#include "hal.h"
#include "dma_hw.h"

// internal representation of an i2c device
struct i2c_t
{
    bool master;                                ///< true if we are a master
    int state;                                  ///< Internal driver state
    I2C_TypeDef *channel;                       ///< i2c channel , ie, I2C1, I2C2
    I2C_InitTypeDef cfg;                        ///< i2c config
    gpio_pin_t *scl,*sda;                       ///< i2c pin

    uint8_t slave_address;                      ///< Slave address
    i2c_error_code_t error_code;                ///< Last error
    i2c_error_cb error_cb;                      ///< called when an error is detected
    void *cb_param;                             ///< passed to user callbacks

    // read buffers
    uint8_t *read_buf;                          ///< buffer to store the read results in
    int16_t read_buf_len;                       ///< size of read buffer
    int16_t read_count;                         ///< number of bytes actually read
    i2c_transfer_complete_cb read_complete_cb;  ///< called when we have read read_buf_len bytes

    // write buffers
    uint8_t *write_buf;                         ///< buffer to transmit
    int16_t write_buf_len;                      ///< number of bytes to transmit
    int16_t write_count;                        ///< number of bytes actually sent
    i2c_transfer_complete_cb write_complete_cb; ///< called when we have transmitted write_buf_len bytes
};

#endif
