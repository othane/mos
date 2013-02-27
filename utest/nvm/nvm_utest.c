/**
 * @file nvm_utest.c
 * 
 * @brief unit test the nvm hal module
 *
 * This test will read the initial "hello" message in flash 
 * and change it to "world". Then the debugger can connect at
 * any time to check flash contains "world"
 *
 * @author OT
 *
 * @date Feb 2013
 *
 */


///@todo use compiler.h to locate the msg var
const char msg[32] @ "MSG" = "hello";
char msg_ram[32] = {0,};

#include <hal.h>
#include <string.h>

void init(void)
{
	sys_init();
}


void main(void)
{
	init();
	
	// on first boot check the value in msg, if it is "hello" change it to
	// "world" and reboot
	nvm_read(msg_ram, msg, 32);
	if (strcmp(msg_ram, "hello") == 0)
	{
		nvm_erase((void*)msg, 32);
		nvm_write((void*)msg, "world", 5);
		sys_reset();
	}
	
	while (1)
	{
	}
}

