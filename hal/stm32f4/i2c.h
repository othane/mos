/**
 * @file i2c.h
 *
 * @brief I2C Master and SLave interface
 *
 * @author Charles Oram
 *
 * @date May 2018
 *
 */

#ifndef __I2C__
#define __I2C__

typedef enum
{
    I2C_ERROR_NONE      = 0x00,
    I2C_ERROR_BERR      = 0x01,
    I2C_ERROR_ARLO      = 0x02,
    I2C_ERROR_AF        = 0x04,
    I2C_ERROR_OVR       = 0x08,
    I2C_ERROR_DMA       = 0x10,
    I2C_ERROR_TIMEOUT   = 0x20,
    I2C_ERROR_INVEVNT   = 0x40
} i2c_error_code_t;

/**
 * @brief opaque i2c type
 */
typedef struct i2c_t i2c_t;

/**
 * @brief Initialise an i2c device
 * @param i2c i2c device to configure
 */    
void i2c_init(i2c_t *i2c);

/**
 * @brief callback when an i2c read or write completes
 * @param i2c i2c device on which the transfer completed
 * @param buf pointer to the buffer transferred
 * @param len number of bytes actually transferred
 * @param param completion parameter passed into the call to i2c_write or i2c_read
 */
typedef void (*i2c_transfer_complete_cb)(i2c_t *i2c, void *buf, uint16_t len, void *param);

/**
 * @brief callback when an i2c error occurs
 * @param i2c i2c device on which the error occurred
 * @param error_code the error that occurred
 * @param param completion parameter passed into the call to i2c_write or i2c_read
 */
typedef void (*i2c_error_cb)(i2c_t *i2c, i2c_error_code_t error_code, void *param);


/**
 * @brief start a write to an i2c device
 * @param i2c i2c device to write too
 * @param buf buffer to send
 * @param len number of bytes in the buffer
 * @param cb completion callback
 * @param error_cb error callback
 * @param param parameter passed to the completion callback
 * Note that the callbacks are called from the interrupt handler.
 */
int i2c_write(i2c_t *i2c, uint8_t device_address,void *buf, uint16_t len,
        i2c_transfer_complete_cb cb, i2c_error_cb error_cb, void *param);

/**
 * @brief cancel i2c write
 * @param i2c i2c device to cancel write on
 */
void i2c_cancel_write(i2c_t *i2c);
/**
 * @brief clear write buffer
 * @param i2c i2c device clear write
 */
void i2c_clear_write(i2c_t *i2c);

/**
 * @brief clear read buffer
 * @param i2c i2c device clear write
 */
void i2c_clear_read(i2c_t *i2c);

/**
 * @brief start an I2C read
 * @param len number of bytes in the buffer
 * @param cb completion callback
 * @param error_cb error callback
 * @param param parameter passed to the completion callback
 * Note that the callbacks are called from the interrupt handler.
 */
int i2c_read(i2c_t *i2c, uint8_t device_address, void *buf, uint16_t len,
        i2c_transfer_complete_cb cb, i2c_error_cb error_cb, void *param);

/**
 * @brief cancel i2c read
 * @param i2c i2c device to cancel read on
 */
void i2c_cancel_read(i2c_t *i2c);


/**
 * @brief Return the last i2c error
 * @param i2c i2c device to get the error code for
 */
i2c_error_code_t i2c_last_error(i2c_t *i2c);

#endif
