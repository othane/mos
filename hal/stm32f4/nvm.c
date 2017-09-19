/**
 * @file nvm.c
 *
 * @brief impliment the nvm module for the stm32f4
 *
 * @author OT
 *
 * @date Feb 2013
 *
 */

#include <stm32f4xx_conf.h>
#include "hal.h"
#include <string.h> // for memcpy (do this manually to remove dep)


static bool erase(uint32_t sector)
{
	bool ret;
	FLASH_Unlock();

	switch (FLASH_EraseSector(sector << 3, VoltageRange_3))
	{
		case FLASH_COMPLETE:
			ret = true;
			break;

		default:
			///@todo error message
			ret = false;
			break;
	}

	FLASH_Lock();
	return ret;
}


static const uint32_t sector_size[] = {
	0x04000, 0x04000, 0x04000, 0x04000, 0x10000, 0x20000, 0x20000, 0x20000,
	0x20000, 0x20000, 0x20000, 0x20000
};


uint32_t nvm_erase(void *addr, uint32_t len)
{
	int n;
	uint32_t addr_start = (uint32_t)addr;
	uint32_t addr_end = (uint32_t)addr + len;
	uint32_t sector_addr = NVM_START_ADDRESS;
	uint32_t bytes_erased = 0;

	// sanity check address range
	if (addr_start < NVM_START_ADDRESS || addr_end > NVM_END_ADDRESS)
		return 0;

	// since we cannot read from the flash during a write/erase cycle
	// letting interrupts run could cause a read and an error, or it 
	// could cause the unlock to fail, so lets do the lot in a critical
	// section
	sys_enter_critical_section();

	// erase any sectors within the start and end address space
	for (n = 0; n < sizeof(sector_size) / sizeof(uint32_t); n++)
	{
		if (sector_addr >= addr_start && sector_addr < addr_end)
		{
			erase(n);
			bytes_erased += sector_size[n];
		}
		sector_addr += sector_size[n];
	}

	sys_leave_critical_section();
	return bytes_erased;
}


bool nvm_read(void *dst, const void *src, uint32_t len)
{
	// on the stm32's the flash can be read directly
	memcpy(dst, src, len);
	return true;
}


#define FLASH_WRITE_TIMEOUT 0x2000
static bool flash_wait(void)
{
	FLASH_Status status = FLASH_BUSY;

	while (status == FLASH_BUSY)
		status = FLASH_WaitForLastOperation();

	if (status != FLASH_COMPLETE)
		///todo error!
		return false;

	return true;
}


static bool flash_write_word(uint16_t *dst, uint16_t *src)
{
	// check if the bytes already match then don't change them (I think we might even get an error)
	if (*dst == *src)
		return true;

	// write the word
	switch (FLASH_ProgramHalfWord((uint32_t)dst, *src))
	{
		case FLASH_COMPLETE:
			// job done lets verify
			break;
		case FLASH_BUSY:
			if (!flash_wait())
				///@todo error
				return false;
			// job done lets verify
			break;

		default:
			///@todo error
			return false;
	}

	// verify (now they should be the same)
	if (*dst != *src)
		///@todo verify error
		return false;  // verification error

	// success
	return true;
}


bool nvm_write(void *dst, const void *src, uint32_t len)
{
	uint16_t *_src = (uint16_t *)src, *_dst = dst;
	bool r = false;

	// sanity checks
	if ((uint32_t)_dst % 2)
		///@todo page must be 2 byte aligned
		return false;

	// since we cannot read from the flash during a write/erase cycle
	// letting interrupts run could cause a read and an error, or it 
	// could cause the unlock to fail, so lets do the lot in a critical
	// section
	sys_enter_critical_section();

	// wait for the flash to be free, unlock it, and wait for it to be free again for the writing
	if (!flash_wait())
		goto done;
	FLASH_Unlock();
	if (!flash_wait())
		goto done;

	// write the bytes as 16bit words
	while (len > 1)
	{
		// write the next word
		if (!flash_write_word(_dst, _src))
			goto done;
		len -= 2;
		_dst++;
		_src++;
	}

	// if we have an odd number of bytes then do the last one
	if (len == 1)
	{
		uint16_t last_word = *_src;
		last_word |= 0xFF00; // little endian (0xFF is erased state)
		if (!flash_write_word(_dst, &last_word))
			goto done;
	}

	// job done !
	r = true;
done:
	sys_leave_critical_section();
	return r;
}

