/**
 * @file gpio_hw.h
 * 
 * @brief this contains hw definitions for configuration via hw.c only (it is not a run time interface !)
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */

#ifndef __GPIO_HW__
#define __GPIO_HW__


#include "hal.h"


// internal representation of a gpio pin
struct gpio_pin_t
{
	uint32_t rcc_periph;		///< used need to enable the clk to the port via RCC_APB2PeriphClockCmd (all gpio are on APB2)
	GPIO_InitTypeDef cfg;		///< used to configure the port via GPIO_Init()
	GPIO_TypeDef *port;			///< port addresses
	uint16_t mask;				///< mask into the port to select this pin
};


#endif

