/**
 * @file gpio.c
 *
 * @brief impliment the gpio module for the stm32107
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */


#include "hal.h"
#include "gpio_hw.h"


void gpio_init_pin(gpio_pin_t *pin)
{
	// module should ignore null pins
	if (pin == NULL)
		return;

	// all gpio pins on the stm32f107x are connected to APB2 so this will be ok
	///@todo should we check if it is enabled before re-enabling ?
	RCC_APB2PeriphClockCmd(pin->rcc_periph, ENABLE);

	// setup the pin configurations 
	GPIO_Init(pin->port, &pin->cfg);
}


void gpio_set_pin(gpio_pin_t *pin, bool state)
{
	// module should ignore null pins
	if (pin == NULL)
		return;

	// check we are in a valid mode to set the pin output
	switch (pin->cfg.GPIO_Mode)
	{
		case GPIO_Mode_AIN:
		case GPIO_Mode_IN_FLOATING:
		case GPIO_Mode_IPD:
		case GPIO_Mode_IPU:
		case GPIO_Mode_AF_OD:
		case GPIO_Mode_AF_PP:
			///@todo set a system error
			return;

		case GPIO_Mode_Out_OD:
		case GPIO_Mode_Out_PP:
			// this are the only valid modes in which we can set the pin output
			break;
	}

	// change the pin output
	if (state)
		// set hi
		pin->port->ODR |= pin->cfg.GPIO_Pin;
	else
		// set lo
		pin->port->ODR &= ~pin->cfg.GPIO_Pin;
}

// this toggles at about 58KHz max, see gpio utest
void gpio_toggle_pin(gpio_pin_t *pin)
{
	// module should ignore null pins
	if (pin == NULL)
		return;

	// check we are in a valid mode to set the pin output
	switch (pin->cfg.GPIO_Mode)
	{
		case GPIO_Mode_AIN:
		case GPIO_Mode_IN_FLOATING:
		case GPIO_Mode_IPD:
		case GPIO_Mode_IPU:
		case GPIO_Mode_AF_OD:
		case GPIO_Mode_AF_PP:
			///@todo set a system error
			return;

		case GPIO_Mode_Out_OD:
		case GPIO_Mode_Out_PP:
			// this are the only valid modes in which we can set the pin output
			break;
	}

	// toggle the pin
	pin->port->ODR ^= pin->cfg.GPIO_Pin;
}


bool gpio_get_pin(gpio_pin_t *pin)
{
	// module should ignore null pins
	if (pin == NULL)
		return false;

	// check we are in a valid mode to set the pin output
	switch (pin->cfg.GPIO_Mode)
	{
		case GPIO_Mode_AIN:
		case GPIO_Mode_AF_OD:
		case GPIO_Mode_AF_PP:
			///@todo set a system error
			return false;

		case GPIO_Mode_Out_OD:
		case GPIO_Mode_Out_PP:
		case GPIO_Mode_IN_FLOATING:
		case GPIO_Mode_IPD:
		case GPIO_Mode_IPU:
			// this are the only valid modes in which we can read the pin value
			break;
	}

	// return the state of the pin
	return ((pin->port->ODR & pin->cfg.GPIO_Pin) != 0);
}

