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
	GPIO_TypeDef *port;             ///< port addresses
	GPIO_InitTypeDef cfg;           ///< used to configure the port via GPIO_Init()
	uint8_t af;						///< select the alternate function to use if cfg.GPIO_Mode == GPIO_Mode_AF
	gpio_edge_event rising_cb;      ///< if not NULL then called on rising edge
	void *rising_cb_param;          ///< passed to rising cb
	gpio_edge_event falling_cb;     ///< if not NULL thane called on falling edge
	void *falling_cb_param;         ///< passed to falling cb
	uint8_t preemption_priority;    ///< lower is a higher priority
	uint8_t pos;
};


#endif

