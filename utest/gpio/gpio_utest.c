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


void falling_edge(gpio_pin_t *pin, void *param)
{
	sys_nop();
}

void rising_edge(gpio_pin_t *pin, void *param)
{
	sys_nop();
}


void init(void)
{
	sys_init();
	
	gpio_init_pin(&gpio_mco);
	gpio_init_pin(&gpio_pa10);
	gpio_init_pin(&gpio_pa15);
	
	gpio_set_falling_edge_event(&gpio_pa15, falling_edge, NULL);
	gpio_set_rising_edge_event(&gpio_pa15, rising_edge, NULL);
}


void main(void)
{
	volatile bool pa15_state;

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
		pa15_state = gpio_get_pin(&gpio_pa15);
	}
}

