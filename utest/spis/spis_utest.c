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


void init(void)
{
	sys_init();
	spis_init(&spis3);
}


#define WRITE_BUF_LEN 6
#define READ_BUF_LEN 3
static uint8_t write_buf[WRITE_BUF_LEN] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
static uint8_t read_buf[6] = {0,};


void read_complete(spis_t *spis, void *buf, uint16_t len, void *param);
void write_complete(spis_t *spis, void *buf, uint16_t len, void *param)
{
	// keep reading
	spis_write(&spis3, write_buf, WRITE_BUF_LEN, write_complete, NULL);
	spis_read(&spis3, read_buf, READ_BUF_LEN, read_complete, NULL);
}


void read_complete(spis_t *spis, void *buf, uint16_t len, void *param)
{
  	if (((uint8_t *)buf)[1] == 0x00)
	  sys_nop();
	// echo back	
	//memcpy(&write_buf[READ_BUF_LEN], buf, len);
}


void main(void)
{
	init();

	// kick off io
	spis_write(&spis3, write_buf, WRITE_BUF_LEN, write_complete, NULL);
	spis_read(&spis3, read_buf, READ_BUF_LEN, read_complete, NULL);
	
	// do nothing
	while (1)
	{
	}
}

