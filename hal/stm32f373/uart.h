/**
 * @file uart.h
 *
 * @brief interface to the uart of the hal
 *
 * @author OT
 *
 * @date Feb 2017
 *
 */


#ifndef __UART__
#define __UART__

/**
 * @brief opaque uart type
 */
typedef struct uart_t uart_t;


/**
 * @brief set a uart device
 * @param uart uart device to configure
 */
void uart_init(uart_t *uart);


/**
 * @brief set read timeout
 * @param uart uart device to set timeout for
 * @param timeout time of inactivity to consider read completed
 */
void uart_set_read_timeout(uart_t *uart, float timeout);


/**
 * @brief callback when a uart write completes
 * @param uart uart device on which the write completed
 * @param buf pointer to the buffer written
 * @param len number of bytes actually written
 * @param param completion parameter passed into the call to uart_write 
 */
typedef void (*uart_write_complete_cb)(uart_t *uart, void *buf, uint16_t len, void *param);


/**
 * @brief start a write to a uart buffer
 * @param uart uart device to write too
 * @param buf buffer to send
 * @param len number of bytes in the buffer
 * @param cb completion callback
 * @param param parameter passed to the completion callback
 */
void uart_write(uart_t *uart, void *buf, uint16_t len, uart_write_complete_cb cb, void *param);


/**
 * @brief return the number of bytes written so far
 * @param uart uart device being written to
 */
int uart_write_count(uart_t *uart);


/**
 * @brief cancel uart write
 * @param uart uart device to cancel write on
 */
void uart_cancel_write(uart_t *uart);


/**
 * @brief callback when a uart read completes
 * @param uart uart device on which the read completed
 * @param buf pointer to the buffer with the read bytes 
 * @param len number of bytes actually read
 * @param param completion parameter passed into the call to uart_read
 */
typedef void (*uart_read_complete_cb)(uart_t *uart, void *buf, uint16_t len, void *param);


/**
 * @brief start a read on a uart buffer
 * @param uart uart device to read from
 * @param buf pointer to a buffer to store the read results in
 * @param len number of bytes in the buffer
 * @param cb completion callback
 * @param param parameter passed to the completion callback
 */
void uart_read(uart_t *uart, void *buf, uint16_t len, uart_read_complete_cb cb, void *param);


/**
 * @brief return the number of bytes read so far
 * @param uart uart device being read from
 */
int uart_read_count(uart_t *uart);


/**
 * @brief reset the whole uart
 * @param uart uart device being reset
 */
void uart_reset(uart_t *uart);


/**
 * @brief cancel uart read
 * @param uart uart device to cancel read on
 */
void uart_cancel_read(uart_t *uart);

/**
 * @brief modify baud rate on-the-fly
 * @param uart uart device to modify_baud
 * @param baud desired new baud rate
 */
void uart_set_baudrate(uart_t *uart, uint32_t baud);

#endif
