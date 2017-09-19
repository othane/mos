/**
 * @file adc_utest.c
 *
 * @brief unit test the adc hal module
 *
 * This test is designed to check the adc module.
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */

#include <string.h>
#include <hal.h>

volatile uint16_t buf[4096];
volatile uint32_t val = 0;

volatile void *vmemset(volatile void *s, int8_t c, size_t n)
{
	volatile unsigned char *p = s;
	while (n--)
		*p++ = (unsigned char)c;
	return s;
}

void init(void)
{
	sys_init();
	adc_channel_init(adc_chan);
	vmemset(buf, 0x00, sizeof(buf));
}

void adc_handle_buffer(volatile uint16_t *dst, int len)
{
	sys_nop();
}

void adc_complete(adc_channel_t *ch, volatile int16_t *dst, int len, void *param)
{
	int n;
	for (n=0; n < len; n++)
		buf[n] += 32767;

	val = adc_read(ch);
	adc_handle_buffer((volatile uint16_t *)dst, len);

	vmemset(buf, 0x00, sizeof(buf));
	adc_trace(ch, (volatile int16_t *)buf, sizeof(buf)/sizeof(uint16_t), 0, adc_complete, param);
}

int main(void)
{
	init();

	val = adc_read(adc_chan);
	adc_trace(adc_chan, (volatile int16_t *)buf, sizeof(buf)/sizeof(uint16_t), 0, adc_complete, NULL);
	while (1)
	{}

	return 0;
}

