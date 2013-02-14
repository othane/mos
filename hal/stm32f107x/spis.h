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


#ifndef __SPIS__
#define __SPIS__


/**
 * @brief opaque slave spi type
 */
typedef struct spis_t spis_t;


/**
 * @brief setup the slave spi device
 */
void spis_init(spis_t *spis);


/**
 * @brief callback called when chip select goes lo
 * @param spis device on which the chip select dropped
 */
typedef void (*spis_select_cb)(spis_t *spis, void *param);


/**
 * @brief callback called when chip select goes hi
 * @param spis device on which the chip select rose
 */
typedef void (*spis_deselect_cb)(spis_t *spis, void *param);


/**
 * @brief set the callback called when chip select goes lo
 * @param spis the device to register this event with
 * @param select called when the chip select line goes lo
 * @param param pass this to the callback when it firres
 */
void spis_set_select_cb(spis_t *spis, spis_select_cb select_cb, void *param);


/**
 * @brief set the callback called when chip select goes hi
 * @param spis the device to register this event with
 * @param deselect called when the chip select line goes hi
 * @param param pass this to the callback when it firres
 */
void spis_set_deselect_cb(spis_t *spis, spis_deselect_cb deselect_cb, void *param);


/**
 * @brief callback when a spis has an error
 * @param spis spis slave device on which the error occured
 * @param param parameter passed from the register function to the error callback
 */
enum SPIS_ERR
{
	SPIS_ERR_UNKNOWN,
	SPIS_ERR_OVRRUN,
	SPIS_ERR_UNDRUN,
};
typedef void (*spis_error_cb)(spis_t *spis, enum SPIS_ERR ecode, void *param);


/**
 * @brief set the callback called on an spis error like overrun or underrun
 * @param spis spis slave device to set the error callback on
 * @param param pass this to the callback if it firres
 */
void spis_set_error_cb(spis_t *spis, spis_error_cb cb, void *param);


/**
 * @brief callback when a spis read completes
 * @param spis spis slave device on which the read completed
 * @param buf pointer to a buffer with the read results in
 * @param len number of bytes actually read
 * @param param completion parameter passed into the call to spis_read
 */
typedef void (*spis_read_complete)(spis_t *spis, void *buf, uint16_t len, void *param);


/**
 * @brief start a asynchronous read on the spis bus
 * @param spis spis slave device to start reading from
 * @param buf pointer to a buffer to store the read results in
 * @param len number of bytes to read before cb is called
 * @param cb completion callback, this is called when number of bytes is finished reading or the transaction ends (nss goes hi)
 * @param param parameter passed to the completion callback
 */
void spis_read(spis_t *spis, void *buf, uint16_t len, spis_read_complete cb, void *param);


/**
 * @brief cancel a read operation
 * @param spis spis slave device to cancel the read for
 */
void spis_cancel_read(spis_t *spis);


/**
 * @brief callback when a spis write completes
 * @param spis spis slave device on which the write completed
 * @param buf pointer to a buffer written
 * @param len number of bytes actually written 
 * @param param completion parameter passed into the call to spis_write
 */
typedef void (*spis_write_complete)(spis_t *spis, void *buf, uint16_t len, void *param);


/**
 * @brief load data to write out the spis bus asynchronously 
 * @param spis spis slave device to start writing to
 * @param buf buffer to send
 * @param len number of bytes in the buffer (if spis transaction continues return empty bits)
 * @param cb completion callback, this is called when number of bytes is sent or the transaction ends (nss goes hi)
 * @param param parameter passed to the completion callback
 */
void spis_write(spis_t *spis, void *buf, uint16_t len, spis_write_complete cb, void *param);


/**
 * @brief cancel a write operation
 * @param spis spis slave device to cancel the write for
 */
void spis_cancel_write(spis_t *spis);


/**
 * @brief check if the spis is selected
 * @param spis, the slave spi device to check if selected
 * @return true if selected, otherwise false
 */
bool spis_selected(spis_t *spis);


#endif

