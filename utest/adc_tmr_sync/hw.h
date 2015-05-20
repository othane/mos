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

extern adc_channel_t adc_chanA;
extern adc_channel_t adc_chanB;
extern adc_channel_t adc_chanC;
extern adc_channel_t adc_chanD;

extern pwm_channel_t pwm0;
extern pwm_channel_t pwm1;
extern pwm_channel_t pwm2;

#endif

