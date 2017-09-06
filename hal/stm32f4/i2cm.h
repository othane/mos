/**
 * @file i2cm.h
 *
 * @brief interface to the i2c master modules of the hal
 *
 * @author OT
 *
 * @date Jan 2017
 *
 */


#ifndef __I2CM__
#define __I2CM__


/**
 * @brief opaque master i2c type
 */
typedef struct i2cm_t i2cm_t;


/**
 * @brief opaque master i2c transfer options
 */
typedef struct i2cm_xfer_opts i2cm_xfer_opts;


/**
 * @brief callback when a i2cm read completes
 * @param i2cm i2c master used
 * @param addr address of the slave
 * @param buf read buffer
 * @param len number of bytes read into the read_buf
 * @param param completion parameter
 */
typedef void (*i2cm_read_complete_t)(i2cm_t *i2cm, uint16_t addr, void *read_buf, uint16_t len, void *param);


/**
 * @brief i2cm_read
 * @param i2cm i2c master to read with
 * @param addr address of the slave to read from
 * @param buf read buffer
 * @param len number of bytes to read into the read_buf
 * @param complete completion callback
 * @param param completion parameter
 */
int i2cm_read(i2cm_t *i2cm, uint16_t addr, void *buf, int len, i2cm_read_complete_t complete, void *param);


/**
 * @brief setup the master i2c device
 * @param i2cm i2c master to init
 */
void i2cm_init(i2cm_t *i2cm);


#endif

