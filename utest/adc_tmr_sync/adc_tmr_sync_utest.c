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

adc_channel_t *chan = &adc_chanC;
volatile uint16_t buf[4096];

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
	pwm_init(&pwm0);
	pwm_init(&pwm1);
	pwm_init(&pwm2);
	adc_channel_init(chan);
	vmemset(buf, 0x00, sizeof(buf));
}

void adc_handle_buffer(volatile uint16_t *dst, int len)
{
	sys_nop();
}

void adc_complete(adc_channel_t *ch, volatile int16_t *dst, int len, void *param)
{
	int n;
	
	//pwm_stop(&pwm2);
	//pwm_stop(&pwm0);
	//pwm_reset(&pwm2);
	//pwm_reset(&pwm0);

	for (n=0; n < len; n++)
		buf[n] += 32767;

	adc_handle_buffer((volatile uint16_t *)dst, len);

	vmemset(buf, 0x00, sizeof(buf));
	adc_trace(ch, (volatile int16_t *)buf, sizeof(buf)/sizeof(uint16_t), 1, adc_complete, param);
	//pwm_start(&pwm2);
	//pwm_start(&pwm0);
}

int main(void)
{
	init();

	adc_trace(chan, (volatile int16_t *)buf, sizeof(buf)/sizeof(uint16_t), 1, adc_complete, NULL);
	pwm_start(&pwm2);
	sys_spin(100);
	pwm_start(&pwm1);
	pwm_start(&pwm0);

	while (1)
	{}

	return 0;
}

