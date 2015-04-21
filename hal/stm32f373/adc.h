/**
 * @file adc.h
 *
 * @brief interface to adc modules of the hal
 *
 * @author OT
 *
 * @date Jan 2014
 *
 */


#ifndef __ADC__
#define __ADC__

/**
 * @brief opaque adc channel
 */
typedef struct adc_channel_t adc_channel_t;


/**
 * @brief read count values from the adc channel given and call cb when done
 * @param ch channel to read from
 * @param dst buffer to store adc vales
 * @param count number of conversions to do (caller must ensure buf is large enough)
 */
typedef void (*adc_trace_complete_t)(adc_channel_t *ch, volatile int16_t *dst, int count, void *param);
void adc_trace(adc_channel_t *ch, volatile int16_t *dst, int count, adc_trace_complete_t cb, void *param);


/**
 * @brief read a single value from the adc channel
 * @param ch channel to read from
 */
int32_t adc_read(adc_channel_t *ch);


/**
 * @brief init this adc channel so it is ready for reads
 * @param ch channel to init
 */
void adc_channel_init(adc_channel_t *ch);

#endif
