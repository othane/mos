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
 * @brief callback on pin logic level edge
 * @param pin pin that saw a edge
 * @param param event parameter passed in to gpio_set_rising|falling_edge_event
 */
typedef void (*gpio_edge_event)(gpio_pin_t *pin, void *param);


/**
 * @brief register a rising edge handler
 * @param pin pin to register this callback with
 * @param cb run this when the pin see's a rising edge
 * @param param completion parameter passed into cb
 */
void gpio_set_rising_edge_event(gpio_pin_t *pin, gpio_edge_event cb, void *param);


/**
 * @brief register a falling edge handler
 * @param pin pin to register this callback with
 * @param cb run this when the pin see's a falling edge
 * @param param completion parameter passed into cb
 */
void gpio_set_falling_edge_event(gpio_pin_t *pin, gpio_edge_event cb, void *param);


/**
 * @brief set a gpio pin hi or lo
 * @param pin the pin from gpio_init_pin to set the value for
 * @param state set pin hi (1) or lo (0)
 */
void gpio_set_pin(gpio_pin_t *pin, uint8_t state);


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

// Pull-up setting
typedef enum
{
    GPIO_NO_PULL = 0,
    GPIO_PULL_UP = 1,
    GPIO_PULL_DOWN = 2
} gpio_pull_type_t;

/**
 * @brief Set the pull-up configuration for a GPIO pin
 * @param pin the pin from gpio_init_pin to toggle
 * @param pull the pull-up setting, either GPIO_NO_PULL,
 *    GPIO_PULL_UP, or GPIO_PULL_DOWN
 */
void gpio_set_pull(gpio_pin_t *pin, gpio_pull_type_t pull);

#endif
