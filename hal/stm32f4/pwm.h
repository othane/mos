/**
 * @file pwm.h
 *
 * @brief implement the pwm module for the stm32f373
 *
 * @author OT
 *
 * @date Apr 2015
 *
 */


#ifndef __PWM__
#define __PWM__

/**
 * @brief opaque pwm channel
 */
typedef struct pwm_channel_t pwm_channel_t;


/**
 * @brief start the pwm (starts the whole timer)
 * @param pwm the pwm channel to start
 *
 */
void pwm_start(pwm_channel_t *pwm);


/**
 * @brief stop the pwm (stops the whole timer)
 * @param pwm the pwm channel to stop
 *
 */
void pwm_stop(pwm_channel_t *pwm);


/**
 * @brief reset the pwm (resets the whole timer)
 * @param pwm the pwm channel to stop
 *
 */
void pwm_reset(pwm_channel_t *pwm);


/**
 * @brief change the pwm frequency
 * @param pwm the pwm channel to change the frequency of
 * @param freq new frequency in hz
 * @note this change effects the whole timer !
 * @return actual frequency found in Hz
 */
float pwm_set_freq(pwm_channel_t *pwm, float freq);


/**
 * @brief change the pwm duty cycle
 * @param pwm the pwm channel to change the duty cycle of
 * @param phs the new phase of the clock where 0 is 0 degrees and 1 is pi radians
 */
void pwm_set_duty(pwm_channel_t *pwm, float duty);


/**
 * @brief setup the pwm timer sync
 * @see tmr_sync_cfg
 * @param pwm the pwm associated with timer to setup the sync on
 * @param ext_clk_mode 0 means timer is clocked from internal clock, 1 means external clock (see ext_clk_freq)
 * @param sync_mode if sync mode is 0 run like a normal timer, otherwise use the slave_mode for sync
 */
void pwm_sync_cfg(pwm_channel_t *pwm, uint8_t ext_clk_mode, uint8_t sync_mode);


/**
 * @brief inits the pwm 
 * @param pwm channel to initialise
 */
void pwm_init(pwm_channel_t *pwm);

#endif


