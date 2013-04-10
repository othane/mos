/**
 * @file bootstrap_main.c
 *
 * @brief select best application to boot
 *
 * @author OT
 *
 * @date Aprl 2013
 *
 */


#include <stm32f10x_conf.h>
#include "hal.h"


static void * get_boot_addr(void)
{
	union
	{
		uint16_t s[2];
		uint32_t *p;
	} _addr;

	PWR_BackupAccessCmd(ENABLE);
	// BKP_DR[1:2] are reserved for boot address
	_addr.s[0] = BKP_ReadBackupRegister(BKP_DR1);
	_addr.s[1] = BKP_ReadBackupRegister(BKP_DR2);
	PWR_BackupAccessCmd(DISABLE);

	return _addr.p;
}


typedef void (*vecttab)(void);
vecttab isr_vector_ram[128] at_symbol(".isr_vector_ram");
static void boot(bootstrap_prog_header *header)
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


const bootstrap_prog_header *bootstrap_bootloader_header at_symbol(".bootstrap_bootloader_header");
const bootstrap_prog_header *bootstrap_program_headers[1] at_symbol(".bootstrap_program_headers");

void main(void)
{
	bootstrap_prog_header *req_prog_header;
	sys_init();

	// see if we were asked to boot a specific program
	req_prog_header = get_boot_addr();

	// try to load a program in this order: requested, default app, bootloader
	if (validate_prog(req_prog_header))
		// we were requested to boot in to this app
		boot(req_prog_header);
	else if (validate_prog(bootstrap_program_headers[0]))
		// for now the list is can only contain 1 prog so just try to boot this
		// @todo for loop over many programs to boot
		boot(bootstrap_program_headers[0]);
	else if (validate_prog(bootstrap_bootloader_header))
		// no valid app so just go to bootloader (hopefully this works)
		boot(bootstrap_bootloader_header);
	else
		// no valid apps (we are useless like this)
		while(1)
		{}
}

