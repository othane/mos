/**
 * @file pwm_utest.c
 *
 * @brief unit test the pwm hal module
 *
 * This test is designed to check the pwm module with
 * the help of an oscilloscope ... running this test should
 * show a 1KHz PWM on the output pin PB6 (see hw.c for setup)
 *
 * @author OT
 *
 * @date Apr 2015
 *
 */


#include <hal.h>

void init(void)
{
	sys_init();
	pwm_init(&pwm0);
}

int main(void)
{
	init();
	pwm_start(&pwm0);

	while (1)
	{}

	return 0;
}

