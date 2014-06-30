/**
 * @file spis.h
 *
 * @brief interface to the slave spi modules of the hal
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */


#ifndef __SPIM__
#define __SPIM__


/**
 * @brief opaque master spi type
 */
typedef struct spim_t spim_t;


/**
 * @brief setup the master spi device
 * @param spim spi master to init
 */
void spim_init(spim_t *spim);


/**
 * @brief callback when a spim xfer completes
 * @param spim spi master to use
 * @param addr address of the slave to xfer to
 * @param read_len fill this many bytes into the read buf
 * @param write_buf send data from here to the MOSI
 * @param len write & read this many byte to/from the read/write buf's
 * @param param completion parameter
 */
typedef void (*spim_xfer_complete)(spim_t *spim, uint16_t addr, void *read_buf, void *write_buf, uint16_t len, void *param);


/**
 * @brief start a spi master transfer
 * @param spim spi master to use
 * @param addr address of the slave to xfer to
 * @param read_len fill this many bytes into the read buf
 * @param write_buf send data from here to the MOSI
 * @param len write & read this many byte to/from the read/write buf's
 * @param complete call this when len bytes are transfered
 * @param param complete parameter
 */
void spim_xfer(spim_t *spim, uint16_t addr, void *read_buf, void *write_buf, int len, spim_xfer_complete complete, void *param);


/**
 * @brief flush the current spi master xfer
 * @param spim spi master to flush
 */
void spim_flush(spim_t *spim);


#endif

