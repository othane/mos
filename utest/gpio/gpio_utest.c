/**
 * @file gpio_utest.c
 * 
 * @brief unit test the gpio hal module
 *
 * This test is designed to check the gpio module.
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */


#include <hal.h>


void init(void)
{
	sys_init();
	
	gpio_init_pin(&gpio_mco);
	gpio_init_pin(&gpio_pa10);
}


void main(void)
{
	init();
	
	// set the pin high, check it is high
	gpio_set_pin(&gpio_pa10, 1);
	while (gpio_get_pin(&gpio_pa10) == 0)
	{}
	
	// set the pin low, check it is low
	gpio_set_pin(&gpio_pa10, 0);
	while (gpio_get_pin(&gpio_pa10) == 1)
	{}

	// toggle the pin ad fast as possible
	while (1)
	{
		gpio_toggle_pin(&gpio_pa10);
	}
}

