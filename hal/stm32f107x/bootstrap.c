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


void bootstrap_set_boot_pid(uint16_t pid)
{
	PWR_BackupAccessCmd(ENABLE);
	// BKP_DR1 is reserved for boot pid
	BKP_WriteBackupRegister(BKP_DR1, pid);
	PWR_BackupAccessCmd(DISABLE);
}


uint16_t bootstrap_get_boot_pid(void)
{
	uint16_t pid;

	PWR_BackupAccessCmd(ENABLE);
	// BKP_DR1 is reserved for boot pid
	pid = BKP_ReadBackupRegister(BKP_DR1);
	PWR_BackupAccessCmd(DISABLE);

	return pid;
}


bool bootstrap_validate_prog(const bootstrap_prog_header *header)
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


/**
 * @brief boot the program described in the program header
 * @param header points to a header describing the program to boot
 */
typedef void (*vecttab)(void);
vecttab isr_vector_ram[128] at_symbol(".isr_vector_ram");
void boot(const bootstrap_prog_header *header)
{
	uint8_t *dst, *src;
	uint32_t k;

	// do I need this (copied from examples)
	sys_enter_critical_section();

	// copy vector table for program from flash to ram (do not use memcpy so we dont need clib)
	dst = (uint8_t *)isr_vector_ram;
	src = (uint8_t *)header->isr_vector;
	for (k = 0; k < sizeof(isr_vector_ram); k++)
		*dst++ = *src++;

	// set cpu to run vector table from ram
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0);

	// start program (this will run its startup and toast our stack etc)
	isr_vector_ram[1]();	
}

