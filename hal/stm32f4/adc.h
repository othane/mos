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
 * @param trigger 0 start now, 1 honour any trigger setup in hw.c
 * @param cb call this once count samples have been read into dst
 * @param param pass this to cb on completion
 */
typedef void (*adc_trace_complete_t)(adc_channel_t *ch, volatile int16_t *dst, int count, void *param);
void adc_trace(adc_channel_t *ch, volatile int16_t *dst, int count, int trigger, adc_trace_complete_t cb, void *param);


/**
 * @brief read a single value from the adc channel
 * @param ch channel to read from
 */
int32_t adc_read(adc_channel_t *ch);


/**
 * @brief change the ADC gain on the fly
 * @param ch channel config to change
 * @param gain the gain to set ie x1, x2, etc it will do its best
 */
void adc_set_gain(adc_channel_t *channel, uint8_t gain);


/**
 * @brief init this adc channel so it is ready for reads
 * @param ch channel to init
 */
void adc_channel_init(adc_channel_t *ch);

#endif
