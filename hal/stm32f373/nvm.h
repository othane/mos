/**
 * @file nvm.h
 *
 * @brief interface to the non volatile memory module of the hal
 *
 * @author OT
 *
 * @date Feb 2013
 *
 */


#ifndef __NVM__
#define __NVM__

/**
 * @brief erase pages from addr to len
 * @param addr points to anywhere in the first page to erase
 * @param len number of bytes to erase, if we cross into a new page erase that also
 * @return number of bytes actually erase (may be much more as we can probably only erase massive pages)
 */
uint32_t nvm_erase(void *addr, uint32_t len);


/**
 * @brief read len bytes from src into dst
 * @param dst destination address
 * @param src source address
 * @param len number of bytes to read
 */
bool nvm_read(void *dst, const void *src, uint32_t len);


/**
 * @brief write len bytes from src into dst
 * @param dst destination address
 * @param src source address
 * @param len number of bytes to write
 */
bool nvm_write(void *dst, const void *src, uint32_t len);


#endif

