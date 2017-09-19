/**
 * @file bootstrap.c
 *
 * @brief common routines used by the bootstrap
 *
 * @author OT
 *
 * @date Dec 2013
 *
 */


#include <stm32f4xx_conf.h>
#include "hal.h"


// these pointers are defined by the linker
extern const uint32_t _bootstrap_program_header_count;
extern const bootstrap_prog_header * const _bootstrap_program_headers[];


const struct bootstrap_prog_header * bootstrap_get_program_header(uint8_t pid)
{
	int k; 

	// go through the list looking for this pid (return pointer to header if
	// found else NULL
	for (k = 0; k < _bootstrap_program_header_count; k++)
	{
		if (_bootstrap_program_headers[k] == NULL)
			continue;

		if (pid == _bootstrap_program_headers[k]->pid)
			return _bootstrap_program_headers[k];
	}
	return NULL;
}


void bootstrap_set_boot_pid(uint16_t pid)
{
	PWR_BackupAccessCmd(ENABLE);
	// RTC_BKP_DR0 is reserved for boot pid
	RTC_WriteBackupRegister(RTC_BKP_DR0, pid);
	PWR_BackupAccessCmd(DISABLE);
}


uint16_t bootstrap_get_boot_pid(void)
{
	uint16_t pid;

	PWR_BackupAccessCmd(ENABLE);
	// RTC_BKP_DR0 is reserved for boot pid
	pid = RTC_ReadBackupRegister(RTC_BKP_DR0);
	PWR_BackupAccessCmd(DISABLE);

	return pid;
}


static void crc_setup(void)
{
	static bool do_crc_init = true;
	if (do_crc_init == true)
		crc_init(&stm32f4_crc_h);
	do_crc_init = false;
}


bool bootstrap_validate_prog(const bootstrap_prog_header *header)
{
	uint32_t crc;

	// null header is invalid
	if (header == NULL || header == (void *)0xffffffff)
		return false;

	// check len
	if (header->len == 0)
		return false;
	
	// check type
	if (header->type != BOOTSTRAP_PROG_HEADER)
		return false;
	
	// check the crc
	crc_setup();
	crc = crc_buf(&stm32f4_crc_h, &header->len, header->len - sizeof(header->crc), true);
	if (header->crc != crc)
		return false;
	
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
	vecttab *dst, *src;
	uint32_t k;

	// do I need this (copied from examples)
	sys_enter_critical_section();

	// copy vector table for program from flash to ram (do not use memcpy so we dont need clib)
	dst = isr_vector_ram;
	src = (vecttab *)header->isr_vector;
	for (k = 0; k < 128; k++)
		dst[k] = src[k];

	// set cpu to run vector table from ram
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0);

	// start program (this will run its startup and toast our stack etc)
	isr_vector_ram[1]();	
}

