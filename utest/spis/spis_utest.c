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
uint16_t wcount = 0;
uint16_t rcount = 0;

#define TEST1
void read_complete(spis_t *spis, void *buf, uint16_t len, void *param);

#ifdef TEST1
void write_complete(spis_t *spis, void *buf, uint16_t len, void *param)
{
  	int k;
	wcount++;
	///@todo for some reason we need this delay or the io goes bad ?!?!?
	for (k = 500; k > 0; k--)
	{}
	// keep reading
	spis_write(&spis3, write_buf, WRITE_BUF_LEN, write_complete, NULL);
	spis_read(&spis3, read_buf, READ_BUF_LEN, read_complete, NULL);
}


void read_complete(spis_t *spis, void *buf, uint16_t len, void *param)
{
  	rcount++;
  	if (((uint8_t *)buf)[1] != 0x01)
	  sys_nop();
	
	// echo back	
	memcpy(&write_buf[READ_BUF_LEN], buf, len);
}

void test_init(void)
{
	// kick off io
	spis_write(&spis3, write_buf, WRITE_BUF_LEN, write_complete, NULL);
	spis_read(&spis3, read_buf, READ_BUF_LEN, read_complete, NULL);  
}
#endif


#ifdef TEST2
void test_init(void)
{
  	// read the first 3 bytes
	spis_read(&spis3, read_buf, READ_BUF_LEN, read_complete, NULL);  
}

void read_complete2(spis_t *spis, void *buf, uint16_t len, void *param)
{
  	rcount++;
	// read the first 3 bytes
	spis_read(&spis3, read_buf, READ_BUF_LEN, read_complete, NULL);
}

void read_complete(spis_t *spis, void *buf, uint16_t len, void *param)
{
  	rcount++;
	// read remaining 3 bytes
	spis_read(&spis3, &read_buf[READ_BUF_LEN], READ_BUF_LEN, read_complete2, NULL);
}
#endif

#ifdef TEST3
void write_complete(spis_t *spis, void *buf, uint16_t len, void *param);
void test_init(void)
{
	// write half of the fixed pattern
	spis_write(&spis3, write_buf, 3, write_complete, NULL);
}

void write_complete2(spis_t *spis, void *buf, uint16_t len, void *param)
{
  	rcount++;
	// write half of the fixed pattern
	spis_write(&spis3, write_buf, 3, write_complete, NULL);
}

void write_complete(spis_t *spis, void *buf, uint16_t len, void *param)
{
  	rcount++;
	// write the rest of the fixed pattern
	spis_write(&spis3, &write_buf[3], 3, write_complete2, NULL);
}
#endif


void main(void)
{
	init();
	test_init();

	// do nothing
	while (1)
	{
	}
}

