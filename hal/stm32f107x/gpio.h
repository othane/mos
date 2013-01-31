/**
 * @file gpio.h
 *
 * @brief interface to gpio modules of the hal
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */


#ifndef __GPIO__
#define __GPIO__


/**
 * @brief opaque gpio pin type 
 */
typedef struct gpio_pin_t gpio_pin_t;


/**
 * @brief initalise a gpio pin object from a pin configuration
 * @param pin_cfg pin to initalise
 */
void gpio_init_pin(gpio_pin_t *pin);


/**
 * @brief set a gpio pin hi or lo
 * @param pin the pin from gpio_init_pin to set the value for
 * @param state set pin hi (1) or lo (0)
 */
void gpio_set_pin(gpio_pin_t *pin, bool state);


/**
 * @brief toggle the gpio pin state from lo to hi or lo to hi
 * @param pin the pin from gpio_init_pin to toggle
 */
void gpio_toggle_pin(gpio_pin_t *pin);


/**
 * @brief get the state of the gpio pin, hi or lo
 * @param pin the pin from gpio_init_pin to get the value for
 * @return gpio state hi (1) or lo (0)
 */
bool gpio_get_pin(gpio_pin_t *pin);


#endif
