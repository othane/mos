/**
 * @file hw.h
 *
 * @brief spis hw file for the stm32f1
 *
 * @author OT
 *
 * @date March 2013
 *
 */

#ifndef __HW__
#define __HW__


/**
 * slave spi device available oh this hw
 */
extern spis_t spis_dev;


/**
 * master spi device available oh this hw
 */
extern spim_xfer_opts spim_dev_opts;
extern spim_t spim_dev;

#endif

