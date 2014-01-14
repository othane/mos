/**
 * @file spis_utest.c
 *
 * @brief unit test the slave spi hal module
 *
 * This test is designed to check the spi slave module.
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */


#include <string.h>
#include <hal.h>


#define WRITE_BUF_LEN 8
#define READ_BUF_LEN 8
static uint8_t write_buf[WRITE_BUF_LEN] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
static uint8_t read_buf[READ_BUF_LEN] = {0,};
uint16_t wcount = 0;
uint16_t rcount = 0;


void write_complete(spis_t *spis, void *buf, uint16_t len, void *param)
{
	wcount++;
}


void read_complete(spis_t *spis, void *buf, uint16_t len, void *param)
{
	rcount++;

	// echo back
	memcpy(write_buf, buf, len);
	spis_write(spis, buf, len, write_complete, NULL);
}

void test_init(void)
{
	// kick off io
	spis_write(&spis_dev, write_buf, WRITE_BUF_LEN, write_complete, NULL);
	spis_read(&spis_dev, read_buf, READ_BUF_LEN, read_complete, NULL);
}


void spis_select(spis_t *spis, void *param)
{
	sys_nop();
}


void spis_deselect(spis_t *spis, void *param)
{
	spis_cancel_read(spis);
	spis_cancel_write(spis);
	test_init();
}


void init(void)
{
	sys_init();
	spis_init(&spis_dev);
	spis_set_select_cb(&spis_dev, spis_select, NULL);
	spis_set_deselect_cb(&spis_dev, spis_deselect, NULL);
}


int main(void)
{
	init();
	test_init();

	// do nothing
	while (1)
	{
	}

	return 0;
}

