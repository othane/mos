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
	GPIO_TypeDef *port;			///< port addresses
	GPIO_InitTypeDef cfg;		///< used to configure the port via GPIO_Init()
};


#endif

