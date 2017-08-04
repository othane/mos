/**
 * @file hw.h
 *
 * @brief i2c hw file for the stm32f373
 *
 * @author CC
 *
 * @date Aug 2017
 *
 */

#ifndef __HW__
#define __HW__


/**
 * slave i2c device available oh this hw
 */
extern i2c_t i2c_dev;


/**
 * i2c device available oh this hw
 */
extern i2c_t i2c_dev;
extern gpio_pin_t i2c_busy;
extern gpio_pin_t i2c_alarm;
extern gpio_pin_t i2c_reset; 
#endif

