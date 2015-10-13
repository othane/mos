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
	pwm_init(&pwm1);
	pwm_init(&pwm2);
	pwm_init(&pwm3);
}

int main(void)
{
	init();

	pwm_start(&pwm1);
	sys_spin(2); // adds a phase shift that should go away in SYNC mode
	pwm_start(&pwm0);
	pwm_start(&pwm3);
	pwm_set_freq(&pwm3, 50000); // test overriding hw.c freq and ensure pwm2 updates duty too
		
	while (1)
	{}

	return 0;
}

