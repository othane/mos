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
#define SPIM_LEN 8
static uint8_t spis_write_buf[WRITE_BUF_LEN] = {0x1f, 0xed, 0xcb, 0xa9, 0x87, 0x65, 0x43, 0x21};
static uint8_t spis_read_buf[READ_BUF_LEN] = {0,};
static uint8_t spim_write_buf[SPIM_LEN] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf1};
static uint8_t spim_read_buf[SPIM_LEN] = {0,};
uint16_t wcount = 0;
uint16_t rcount = 0;

static int again = 0;

static void spis_writecomplete(spis_t *spis, void *buf, uint16_t len, void *param)
{
	wcount++;
}


static void spis_readcomplete(spis_t *spis, void *buf, uint16_t len, void *param)
{
	rcount++;

	// echo back
	memcpy(spis_write_buf, buf, len);
	spis_write(spis, buf, len, spis_writecomplete, NULL);
	again = 1;
}


void spis_select(spis_t *spis, void *param)
{
	sys_nop();
}


void spis_deselect(spis_t *spis, void *param)
{
	spis_flush_read(spis);
	spis_flush_write(spis);
	spis_write(&spis_dev, spis_write_buf, WRITE_BUF_LEN, spis_writecomplete, NULL);
	spis_read(&spis_dev, spis_read_buf, READ_BUF_LEN, spis_readcomplete, NULL);
}


void spim_complete(spim_t *spim, uint16_t addr, void *read_buf, void *write_buf, uint16_t len, void *param)
{
	sys_nop();
}


void init(void)
{
	sys_init();
	spis_init(&spis_dev);
	spis_set_select_cb(&spis_dev, spis_select, NULL);
	spis_set_deselect_cb(&spis_dev, spis_deselect, NULL);
	spim_init(&spim_dev);
}


int main(void)
{
	init();
	spis_write(&spis_dev, spis_write_buf, WRITE_BUF_LEN, spis_writecomplete, NULL);
	spis_read(&spis_dev, spis_read_buf, READ_BUF_LEN, spis_readcomplete, NULL);
	spim_xfer(&spim_dev, &spim_dev_opts, 0x01, spim_read_buf, spim_write_buf, SPIM_LEN, spim_complete, NULL);

	// do nothing
	while (1)
	{
		if (again)
		{
			again = 0;
			sys_spin(20);
			spim_xfer(&spim_dev, &spim_dev_opts, 0x01, spim_read_buf, spim_write_buf, SPIM_LEN, spim_complete, NULL);
		}
	}

	return 0;
}

