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


#include "hal.h"


#define PROGRAM_HEADERS 2
const uint32_t bootstrap_program_header_count at_symbol(".bootstrap_program_header_count") = PROGRAM_HEADERS;
const bootstrap_prog_header *bootstrap_program_headers[PROGRAM_HEADERS] at_symbol(".bootstrap_program_headers") = {0xffffffff, 0xffffffff};


int main(void)
{
	uint16_t prog = 0;
	uint16_t boot_pid;

	// basic start up
	sys_init();

	// see if we were asked to boot a specific program
	boot_pid = bootstrap_get_boot_pid();
	if ((boot_pid <= 0) || (boot_pid > PROGRAM_HEADERS))
		goto default_boot;

requested_boot:
	// search the program list for requested boot_pid (if the pid
	// cannot be found do the default boot)
	for (prog = 0; prog < PROGRAM_HEADERS; prog++)
	{
		if (bootstrap_program_headers[prog]->pid == boot_pid)
		{
			if (bootstrap_validate_prog(bootstrap_program_headers[prog]))
				boot(bootstrap_program_headers[prog]);
		}
	}

default_boot:
	// look for the next valid program to run
	for (prog = 0; prog < PROGRAM_HEADERS; prog++)
	{
		if (bootstrap_validate_prog(bootstrap_program_headers[prog]))
			boot(bootstrap_program_headers[prog]);
	}

bricked_boot:
	// no valid programs to boot (we are useless like this)
	while(1)
	{}

	// keep gcc happy
	return 0;
}

