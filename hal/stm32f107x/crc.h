/**
 * @file crc.h
 *
 * @brief interface to the crc module of the hal
 *
 * @author OT
 *
 * @date March 2013
 *
 */


#ifndef __CRC__
#define __CRC__


/**
 * @brief 8 bit crc 
 * @param buf crc this
 * @param len size of buf in bytes
 */
uint8_t crc8(void *buf, uint32_t len);


/**
 * @brief 16 bit crc 
 * @param buf crc this
 * @param len size of buf in bytes
 */
uint16_t crc16(void *buf, uint32_t len);


/**
 * @brief 32 bit crc 
 * @param buf crc this
 * @param len size of buf in bytes
 */
uint32_t crc32(void *buf, uint32_t len);


#endif

