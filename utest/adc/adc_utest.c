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

adc_trace_point_t buf[4096];
uint32_t val = 0;

void init(void)
{
	sys_init();
	adc_channel_init(adc_chan);
	memset((void *)buf, 0x00, sizeof(buf));
}

void adc_handle_buffer(adc_trace_point_t *dst, int len)
{
	sys_nop();
}

void adc_complete(adc_channel_t *ch, adc_trace_point_t *dst, int len, void *param)
{
	val = adc_read(ch);
	adc_handle_buffer((adc_trace_point_t *)dst, len);

	memset((void *)buf, 0x00, sizeof(buf));
	val = adc_read(ch);
	adc_trace(ch, (adc_trace_point_t *)buf, sizeof(buf)/sizeof(adc_trace_point_t), 0, adc_complete, param);
}

int main(void)
{
	init();

	val = adc_read(adc_chan);
	adc_trace(adc_chan, (adc_trace_point_t *)buf, sizeof(buf)/sizeof(adc_trace_point_t), 0, adc_complete, NULL);
	while (1)
	{}

	return 0;
}

