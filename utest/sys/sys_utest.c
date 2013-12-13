/**
 * @file sys_utest.c
 *
 * @brief unit test the sys hal module
 *
 * This test is designed to check the sys module sets the clks up
 * as desired and that timing is correct in general.
 *
 * FOR STM32F107x:
 * PA8 should output the sys_clk / 2 which can be checked via scope to
 *  ensure the cpu is running at the correct frequency
 * PA10 should output a 1ms hi, 1ms low square wave generated via the
 *  1ms sys_tick function. Scoping PA10 will ensure sys_tick is setup
 *  correctly for 1ms.
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */


#include <hal.h>


void init()
{
	sys_init();

	// this utest requires very basic gpio module also
	gpio_init_pin(&gpio_mco);
	gpio_init_pin(&gpio_systick);
}


int main(void)
{
	uint32_t tick = 0, last_tick = 0;
	init();

	while (1)
	{
		tick = sys_get_tick();
		if (tick != last_tick)
		{
			// toggle gpio_systick to dbg sys tick timing issues
			gpio_toggle_pin(&gpio_systick);
		}
		last_tick = tick;
	}

	return 0;
}

