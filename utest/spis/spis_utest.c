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


#define BUF_LEN 3	///@note set the nanoboard to read 3 and write 3 bytes (it is 1/2 duplex)
static uint8_t write_buf[BUF_LEN] = {0x03, 0x04, 0x5};
static uint8_t read_buf[BUF_LEN] = {0,};


void read_complete(spis_t *spis, void *buf, uint16_t len, void *param);
void write_complete(spis_t *spis, void *buf, uint16_t len, void *param)
{
	// keep reading
	spis_read(&spis3, read_buf, BUF_LEN, read_complete, NULL);  
}


void read_complete(spis_t *spis, void *buf, uint16_t len, void *param)
{
	// echo back
	spis_write(spis, write_buf, BUF_LEN, write_complete, NULL);
}


void main(void)
{
	init();

	// kick off a read
	spis_read(&spis3, read_buf, BUF_LEN, read_complete, NULL);
	
	// do nothing
	while (1)
	{
	}
}

