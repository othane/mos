/**
 * @file sdram_utest.c
 *
 * @brief unit test the sdram hal module
 *
 * This test is designed to check the sdram module.
 *
 * @author OT
 *
 * @date Jun 2018
 *
 */

#include <string.h>
#include <hal.h>


#define SDRAM_ADDR (0xd0000000)
volatile uint8_t buf[8*1024*1024] at_symbol(".sdram"); // give me all the bytes :)


void fail(void)
{
	while (1);
}


void read_complete(spis_t *spis, void *buf, uint16_t len, void *param)
{
	// keep going
	spis_read(&spis_dev, (void *)buf, 32, read_complete, NULL);
}


void init(void)
{
	sys_init();
	spis_init(&spis_dev);
	fmc_sdram_init(&sdram);
}


int main(void)
{
	long long n;
	uint8_t k;

	init();

	*(uint8_t *)(SDRAM_ADDR) = 0xee;

	// write all the bytes to the sdram and check they are there
	for (n = 0, k = 0; n < sizeof(buf); n++, k++)
		buf[n] = k;

	for (n = 0, k = 0; n < sizeof(buf); n++, k++)
		if (buf[n] != k)
			fail();

	// wait for a spi packet to be DMA'ed into the buffer
	spis_read(&spis_dev, (void *)buf, 32, read_complete, NULL);
	while (1);

	return 0;
}

