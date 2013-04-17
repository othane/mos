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


static const char msg[40] = "Hello world this is a message to crc\x00\x00\x00\x00";
static uint32_t known_msg_crc = 0xb3c8cbe8;
uint32_t crc_tbl[256];
struct crc_h h =
{
	{32, 0x04C11DB7, 0xFFFFFFFF, FALSE, FALSE, 0, 0},
	crc_tbl,
	sizeof(crc_tbl),
	CRC_METHOD_BEST,
};


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
	
	return crc;
}


#define CRC_MATCH(crc) ((crc == known_msg_crc)? 'p': 'f')
int main(void)
{
	uint32_t crc_soft, crc_tab, crc_best;
	int i;
	char res;

	init();
	
	h.method = CRC_METHOD_SOFT;
	crc_soft = run_crc();
	h.method = CRC_METHOD_TABLE_32W;
	crc_tab  = run_crc();
	h.method = CRC_METHOD_BEST;
	crc_best = run_crc();

	#ifdef PRINT_RESULT
	printf("msg = ");
	for (i = 0; i < sizeof(msg); i++)
	{
		if (msg[i] < 32 || msg[i] > 126)
			printf("\\x%.2X", msg[i]);
		else
			printf("%c", msg[i]);
	}
	printf("\n");
	printf("len = %d\n", sizeof(msg));
	res = CRC_MATCH(crc_soft);
	printf("crc_soft = 0x%.4X [%c]\n", crc_soft, res);
	res = CRC_MATCH(crc_tab);
	printf("crc_tab= 0x%.4X [%c]\n", crc_tab, res);
	res = CRC_MATCH(crc_best);
	printf("crc_best= 0x%.4X [%c]\n", crc_best, res);
	printf("[crc_best method: %d]\n", h.method);
	res = 'f';
	if (crc_soft == crc_tab == crc_best == known_msg_crc)
		res = 'p';
	printf("\ntest result %c\n\n", res);
	#endif

	#ifdef EMBEDDED 
	// done	
	while (1)
	{}
	#endif

	return 0;
}

