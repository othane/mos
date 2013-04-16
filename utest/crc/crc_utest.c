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

#include <crc.h>
#ifdef EMBEDDED 
#include <hal.h>
#endif

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#ifdef PRINT_RESULT
#include <stdio.h>
#endif


static const char msg[] = "Hello world this is a message to crc";
static uint32_t msg_known_crc = 0xb3c8cbe8;
uint32_t crc_tbl[256];
struct crc_h h =
{
	{32, 0x04C11DB7, 0xFFFFFFFF, FALSE, FALSE, 0, 0},
	crc_tbl,
	sizeof(crc_tbl),
	CRC_METHOD_BEST,
};
bool result = false;


void init(void)
{
	#ifdef EMBEDDED
	sys_init();
	#endif

}


uint32_t run_crc(void)
{
	uint32_t crc;
	uint32_t len;

	// mangle the message length to be 4 byte aligned so we can compare HW with other methods
	len = sizeof(msg);
	if (len & 0x03)
		len = (len & 0xFFFFFFFC) + 0x04;
	else
		len = (len & 0xFFFFFFFC);

	crc_init(&h);
	crc = crc_buf(&h, msg, len);
	
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
	
	return crc;
}


int main(void)
{
	uint32_t crc_soft, crc_tab, crc_hard;

	init();
	
	h.method = CRC_METHOD_SOFT;
	crc_soft = run_crc();
	h.method = CRC_METHOD_TABLE_32W;
	crc_tab  = run_crc();
	h.method = CRC_METHOD_BEST;
	crc_hard = run_crc();

	#ifdef EMBEDDED 
	// done	
	while (1)
	{}
	#endif

	return 0;
}

