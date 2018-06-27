/**
 * @file hal.h
 *
 * @brief interface to the mos hal layer
 *
 * @author OT
 *
 * @date Jan 01 2013
 *
 */

#ifndef __HAL__
#define __HAL__


/* hal dependancies */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


/* hal components */
#include "../compiler.h"
#include "sys.h"
#include "dma.h"
#include "gpio.h"
#include "uart.h"
#include "spis.h"
#include "spim.h"
#include "nvm.h"
#include "fmc.h"
#include "crc.h"
#include "adc.h"
#include "tmr.h"
#include "pwm.h"
#include "ppm.h"
#include "usb.h"
#include "i2c.h"
#include "bootstrap.h"

#ifndef NOHW_H
#include <hw.h> 
#endif


#endif

