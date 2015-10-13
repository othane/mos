/**
 * @file tmr.h
 *
 * @brief implement the tmr module for the stm32f373
 *
 * @author OT
 *
 * @date Apr 2015
 *
 */


#ifndef __TMR__
#define __TMR__

/**
 * @brief opaque tmr 
 */
typedef struct tmr_t tmr_t;


/**
 * @brief start the tmr
 * @param tmr the tmr to start
 *
 */
void tmr_start(tmr_t *tmr);


/**
 * @brief stop the tmr
 * @param tmr the timer to stop
 *
 */
void tmr_stop(tmr_t *tmr);


/**
 * @brief is the timer running
 * @param tmr the timer to check
 * @return 0 if timer is stopped, otherwise timer is running
 */
int tmr_running(tmr_t *tmr);


/**
 * reset the timer count
 * @param tmr the timer to reset
 */
void tmr_reset(tmr_t *tmr);


/**
 * @brief change the tmr frequency
 * @param tmr the timer to change the frequency of
 * @param freq new frequency in hz
 * @note this change effects the whole timer !
 * @return actual frequency found in Hz
 */
uint32_t tmr_set_freq(tmr_t *tmr, uint32_t freq);


/**
 * @brief add callback to run for each channel when this timer changes its frequency
 * @param tmr timer to connect the callback
 * @param cb callback function
 * @param channel indicates which channel on the timer this is called for
 * @param param callback parameter
 */
typedef void (*freq_update_cb_t)(tmr_t *tmr, int ch, void *param); // internal callback for pwm/ppm modules etc
void tmr_set_freq_update_cb(tmr_t *tmr, freq_update_cb_t cb, int channel, void *param); // internal function only


/**
 * @brief get the tick count of the timer
 * @param tmr the timer to get the tick count of
 * @return the tick count of the timer
 */
uint32_t tmr_get_tick(tmr_t *tmr);


/**
 * @brief inits the tmr 
 * @param tmr timer to initialise
 */
void tmr_init(tmr_t *tmr);

#endif


