/**
 * @file ppm.h
 *
 * @brief implement the ppm module for the stm32f373
 *
 * @author OT
 *
 * @date Apr 2015
 *
 */


#ifndef __PPM__
#define __PPM__

/**
 * @brief opaque ppm channel
 */
typedef struct ppm_channel_t ppm_channel_t;


/**
 * @brief start the ppm on a given channel
 * @param ppm the ppm channel to start
 *
 */
void ppm_start(ppm_channel_t *ppm);


/**
 * @brief stop the ppm on a given channel
 * @param ppm the ppm channel to stop
 *
 */
void ppm_stop(ppm_channel_t *ppm);


/**
 * @brief change the ppm frequency
 * @param ppm the ppm channel to change the frequency of
 * @param freq new frequency in hz
 * @note this change effects the whole timer !
 * @return actual frequency found in Hz
 */
uint32_t ppm_set_freq(ppm_channel_t *ppm, uint32_t freq);


/**
 * @brief change the ppm phase
 * @param ppm the ppm channel to change the phase of
 * @param phs the new phase of the clock where 0 is 0 degrees and 1 is pi radians
 */
void ppm_set_phs(ppm_channel_t *ppm, float phs);


/**
 * @brief inits the ppm 
 * @param ppm channel to initialise
 */
void ppm_init(ppm_channel_t *ppm);

#endif

