/**
 * @file hw.h
 *
 * @brief default hw available on the stm32 system (pins/peripheral setups etc)
 *
 * @note to override the defaults make your own definitions in your hw.c and include that in the build not the default. Then expose them in your own hw.h (defining __HW__) before inluding mos.h or hal.h
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */

#ifndef __HW__
#define __HW__


/**
 * list of gpio pins available on this hw
 */
extern gpio_pin_t gpio_mco;
extern gpio_pin_t gpio_pa10;
extern gpio_pin_t gpio_pa15;
extern gpio_pin_t gpio_led;


/**
 * list of slave spi device available oh this hw
 */
extern spis_t spis3;


#endif

