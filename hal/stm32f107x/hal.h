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
#include "sys.h"
#include "gpio.h"
#include "spis.h"
#include "nvm.h"

#include "../compiler.h"
#ifndef NOHW_H
#include <hw.h>     ///< this can be the default in the hal/stmf107x dir or your own for a custom system
#endif


#endif

