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


gpio_pin_t *led_list[] = {&gpio_led0, &gpio_led1, &gpio_led2, &gpio_led3};
volatile int l = 0;


void falling_edge(gpio_pin_t *pin, void *param)
{
	if (++l > 3)
		l = 0;
}

void rising_edge(gpio_pin_t *pin, void *param)
{
	if (++l > 3)
		l = 0;
}


void init(void)
{
	sys_init();

	gpio_init_pin(&gpio_led0);
	gpio_init_pin(&gpio_led1);
	gpio_init_pin(&gpio_led2);
	gpio_init_pin(&gpio_led3);
	gpio_init_pin(&gpio_in);

	gpio_set_falling_edge_event(&gpio_in, falling_edge, NULL);
	gpio_set_rising_edge_event(&gpio_in, rising_edge, NULL);
}

int main(void)
{
	volatile bool pa_in_state unused;
	int k;

	init();

	// set all the led pins hi (turns leds off on dev kits)
	for (k = 0; k < sizeof(led_list) / sizeof(gpio_pin_t *); k++)
	{
		gpio_set_pin(led_list[k], 1);
		while (gpio_get_pin(led_list[k]) == 0)
		{}
	}

	// toggle the led list as fast as possible (change led on button press)
	while (1)
	{
		for (k = 0; k < sizeof(led_list) / sizeof(gpio_pin_t *); k++)
		{
			if (k == l)
			{
				gpio_set_pin(led_list[k], 0);
				while (gpio_get_pin(led_list[k]) == 1)
				{}
			}
			else
			{
				gpio_set_pin(led_list[k], 1);
				while (gpio_get_pin(led_list[k]) == 0)
				{}
			}
		}
		pa_in_state = gpio_get_pin(&gpio_in);
	}

	return 0;
}

