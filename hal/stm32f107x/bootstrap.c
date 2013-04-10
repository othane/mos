/**
 * @file bootstrap.c
 *
 * @brief common routines used by the bootstrap
 *
 * @author OT
 *
 * @date Aprl 2013
 *
 */


#include <stm32f10x_conf.h>
#include "hal.h"


bool validate_prog(bootstrap_prog_header *header)
{
	// null header is invalid
	if (header == NULL)
		return false;

	// check len
	if (header->len == 0)
		return false;
	
	// check type
	if (header->type != BOOTSTRAP_PROG_HEADER)
		return false;
	
	// check the crc
	///@todo do this when crc branch is merged
	
	return true;
}


static void set_boot_addr(void *addr)
{
	union
	{
		uint16_t s[2];
		uint32_t *p;
	} _addr;
	_addr.p = addr;

	PWR_BackupAccessCmd(ENABLE);
	// BKP_DR[1:2] are reserved for boot address
	BKP_WriteBackupRegister(BKP_DR1, _addr.s[0]);
	BKP_WriteBackupRegister(BKP_DR2, _addr.s[1]);
	PWR_BackupAccessCmd(DISABLE);
}


bool bootstrap_switch(bootstrap_prog_header *header)
{
	if (!validate_prog(header))
		return false;

	set_boot_addr(header);
	sys_reset();

	// just for completness (we will never acutally return as we reset above)
	return true;
}

