/**
 * @file fmc.h
 *
 * @brief interface to flexible memory controller module of the hal
 *
 * @author OT
 *
 * @date June 2018
 *
 */


#ifndef __FMC__
#define __FMC__

/**
 * @brief opaque sdram
 */
typedef struct fmc_sdram_t fmc_sdram_t;


/**
 * @brief initalise an sdram using the flxible memory controller to map it into the system address space
 * @param sdram sdram chip to use
 * @note this function only works for the stm32f42xxx/stm32f43xxx series
 */
void fmc_sdram_init(fmc_sdram_t *sdram);

#endif
