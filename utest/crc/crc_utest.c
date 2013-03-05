/**
 * @file crc_utest.c
 * 
 * @brief unit test the crc hal module
 *
 * This test will crc a message with both the hardware and
 * software crc methods and ensure they match
 *
 * @author OT
 *
 * @date Feb 2013
 *
 */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#ifdef PRINT_RESULT
#include <stdio.h>
#endif

#ifdef CRC_HARD
#include <hal.h>
#else
#include "crc.h"
#endif


static const char msg[] = "Hello world this is a message to crc";
static uint32_t msg_known_crc = 0xDF750117;


void init(void)
{
	#ifdef CRC_HARD
	sys_init();
	#endif
}


#ifdef CRC_SOFT
uint32_t _crc_soft(void *buf, uint32_t len);
#endif
#ifdef CRC_HARD
uint32_t _crc_hard(void *buf, uint32_t len);
#endif


bool result = false; // assume test fails and prove it passes
void main(void)
{
	uint32_t crc;
	#ifdef CRC_SOFT
	uint32_t crc_soft;
	#endif
	#ifdef CRC_HARD
	uint32_t crc_hard;
	#endif

	init();

	// crc msg
	crc = crc32((void *)msg, sizeof(msg));

	// check unit test results
	if (crc == msg_known_crc)
		result = true;
	#ifdef PRINT_RESULT
	if (result)
		printf("test passed:\n");
	else
		printf("test failed:\n");
	printf("crc = 0x%.4X%\n", crc);
	#endif

	#ifdef CRC_SOFT
	// crc msg via software
	crc_soft = _crc_soft((void *)msg, sizeof(msg));
	#ifdef PRINT_RESULT
	printf("crc_soft = 0x%.4X%\n", crc_soft);
	#endif
	#endif
	
	#ifdef CRC_HARD
	// crc msg via hardware
	crc_hard = _crc_hard((void *)msg, sizeof(msg));
	#ifdef PRINT_RESULT
	printf("crc_hard = 0x%.4X%\n", crc_hard);
	#endif
	#endif

	#ifndef PRINT_RESULT
	// done	
	while (1)
	{
	}
	#endif
}

