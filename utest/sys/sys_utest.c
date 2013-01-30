/**
 * @file sys_utest.c
 * 
 * @brief unit test the sys hal module
 *
 * This test is designed to check the sys module sets the clks up
 * as desired and that timing is correct in general.
 *
 * PA8 should output the sys_clk / 2 which can be check via scope to 
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

void gpio_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	// Configure PA8 as alt function output push pull (MCO)
	//  probe this pin to measure sys_clk / 2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Configure PA10 as GPIO push pull for testing sys_tick timing
	//  this should generate a 0.5KHz square wave on PA8 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void init()
{
	sys_init();
	gpio_init();
}

void main(void)
{
	uint32_t tick = 0, last_tick = 0;
	init();

	while (1)
	{
		tick = sys_get_tick();
		if (tick != last_tick)
		{
			// toggle PA10 to dbg sys tick timing issues
			GPIOA->ODR ^= GPIO_Pin_10;
		}
		last_tick = tick;
	}
}

