/**
 * @file hw.h
 * 
 * @brief hw setup for ppm unit test
 *
 * @note to override the defaults make your own definitions in your hw.c and include that in the build not the default. Then expose them in your own hw.h (defining __HW__) before inluding mos.h or hal.h
 *
 * @author OT
 *
 * @date Apr 2015
 *
 */

#ifndef __HW__
#define __HW__

extern ppm_channel_t ppm_ref;
extern ppm_channel_t ppm_drift;

#endif

