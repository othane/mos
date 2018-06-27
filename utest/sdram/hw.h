/**
 * @file hw.h
 * 
 * @brief default hw available on the stm32f429 disco system for this unit test (pins/peripheral setups etc)
 *
 * @note to override the defaults make your own definitions in your hw.c and include that in the build not the default. Then expose them in your own hw.h (defining __HW__) before inluding mos.h or hal.h
 *
 * @author OT
 *
 * @date Jun 2018
 *
 */

#ifndef __HW__
#define __HW__

extern spis_t spis_dev;
extern fmc_sdram_t sdram;

#endif

