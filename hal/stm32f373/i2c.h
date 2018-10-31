/**
 * @file i2c.h
 *
 * @brief interface to the i2c of the hal
 *
 * @author CC
 *
 * @date Aug 2017
 *
 */

#ifndef __I2C__
#define __I2C__

/**
 * @brief opaque i2c type
 */
typedef struct i2c_t i2c_t;
/**
 * @brief set a i2c device
 * @param i2c i2c device to configure
 */    
void i2c_init(i2c_t *i2c);

/**
 * @brief callback when a i2c write completes
 * @param i2c i2c device on which the write completed
 * @param buf pointer to the buffer written
 * @param len number of bytes actually written
 * @param param completion parameter passed into the call to i2c_write 
 */
typedef void (*i2c_write_complete_cb)(i2c_t *i2c, void *buf, uint16_t len, void *param);

/**
 * @brief start a write to a i2c buffer
 * @param i2c i2c device to write too
 * @param buf buffer to send
 * @param len number of bytes in the buffer
 * @param cb completion callback
 * @param param parameter passed to the completion callback
 */
int i2c_write(i2c_t *i2c, uint8_t device_address,void *buf, uint16_t len, i2c_write_complete_cb cb, void *param);

/**
 * @brief return the number of bytes written so far
 * @param i2c i2c device being written to
 */
int i2c_write_count(i2c_t *i2c);

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
 * @brief callback when a i2c read completes
 * @param i2c i2c device on which the read completed
 * @param buf pointer to the buffer with the read bytes 
 * @param len number of bytes actually read
 * @param param completion parameter passed into the call to i2c_read
 */
typedef void (*i2c_read_complete_cb)(i2c_t *i2c, void *buf, uint16_t len, void *param);

/**
 * @brief start a read on a i2c buffer
 * @param i2c i2c device to read from
 * @param buf pointer to a buffer to store the read results in
 * @param len number of bytes in the buffer
 * @param cb completion callback
 * @param param parameter passed to the completion callback
 */
int i2c_read(i2c_t *i2c, uint8_t device_address,void *buf,uint16_t len, i2c_read_complete_cb cb, void *param);

/**
 * @brief return the number of bytes read so far
 * @param i2c i2c device being read from
 */
int i2c_read_count(i2c_t *i2c);

/**
 * @brief cancel i2c read
 * @param i2c i2c device to cancel read on
 */
void i2c_cancel_read(i2c_t *i2c);

#endif
