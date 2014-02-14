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


#include <hal.h>
#define CHAN adc_chanD


void init(void)
{
	sys_init();
	adc_channel_init(&CHAN);
}

int main(void)
{
	int32_t val unused;

	init();

	while (1)
	{
		val = adc_read(&CHAN);
		sys_spin(1);
	}

	return 0;
}

