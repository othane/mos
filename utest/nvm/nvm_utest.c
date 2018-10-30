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

#include <hal.h>
#include <string.h>

#if defined (stm32f373)
	#define PAGE_SIZE (2 * 1024)
#else // for now default to a 16KB pages for now so you might get a build error if you get this wrong (hopefully, but probably not)
	#define PAGE_SIZE (16 * 1024)
#endif
const char page0[2*PAGE_SIZE] at_symbol(".free_page0") = {1,2,3,4,};
char page0_ram[sizeof(page0)];
const char msg[32] at_symbol(".free_page1") = "hello";
char msg_ram[sizeof(msg)] = {0,};


void init(void)
{
	sys_init();
}


int main(void)
{
	init();

	// on first boot check the value in msg, if it is "hello" change it to
	// "world" and reboot
	nvm_read(msg_ram, msg, 32);
	if (strcmp(msg_ram, "hello") == 0)
	{
		int n;

		// check we can erase 2 pages properly
		nvm_erase((void*)page0, sizeof(page0));
		for (n = 0; n < sizeof(page0_ram); n++)
			page0_ram[n] = 1;
		nvm_write((void*)page0, page0_ram, sizeof(page0));
		nvm_erase((void*)page0, sizeof(page0));

		// check we can write to flash and save it after a power cycle
		nvm_erase((void*)msg, 32);
		nvm_write((void*)msg, "world", 5);
		sys_reset();
	}

	while (1)
	{
	}

	return 0;
}

