/**
 * @file sys.h
 *
 * @brief interface to the system level functions
 *
 * @author OT
 *
 * @date Jan 01 2013
 *
 */

#ifndef __SYS__
#define __SYS__


#define SYS STM32f107

typedef void (*SysTick_Extra_Handler_f)(void);


/**
 * @brief simple set of system wide error codes
 */
enum SYS_ERR
{
	SYS_ERR_NONE = 0, /**< no pending system errors */
};


/**
 * @brief enter a critical section that cannot be interrupted by any other process or isq
 * @see sys_end_critical_section
 * @note this should be re-entrant, ie if called twice in a row, it would take 2 corresponding calls to sys_leave_critical_section to allow processes/isrs to run again
 */
void sys_enter_critical_section(void);


/**
 * @brief leave a critical section, allow other processes/isq
 * @see sys_start_critical_section
 * @note this should be re-entrant, ie if sys_enter_critical_section is called twice in a row, it would take 2 corresponding calls to allow processes/isrs to run again
 */
void sys_leave_critical_section(void);


/**
 * @brief return the system clock frequency
 * @return system clock frequency in Hz
 */
uint32_t sys_clk_freq(void);


/**
 * @brief get the number of 1ms intervals since boot
 * @note there is not attempt to deal with rollovers in this function
 * @return the number of 1ms ticks that have occurred since boot time
 */
uint32_t sys_get_tick(void);


/**
 * @brief return the number of ticks between beginning and end and handle wrapping
 * @param beginning lower bound on the time interval
 * @param end upper bound on the time interval
 * @note this function assumes beginning was recorded before end, @see sys_tick_diff
 * if this is not guaranteed
 */
uint32_t sys_abs_tick_diff(uint32_t beginning, uint32_t end);


/**
 * @brief return the number of ticks t2 is from t1 (if t2 is earlier than t1 result is negative)
 * @param t1 the reference point if t2 is larger this function returns positive, else negative
 * @param t2 "
 * @note this function resolves the inherent ambiguity between t2 being earlier/later than t1 by
 * trying to minimise the tick difference, ie if t1 = 11 and t2 = 10 the result would be -1, not 
 * MAX_TICKS-1. This is simply the most useful way to resolve this ambiguity
 * @see sys_abs_tick_diff for an alternative method (only positive)
 */
int32_t sys_tick_diff(uint32_t t1, uint32_t t2);


/**
 * @brief call func with args in argv
 * @param func function pointer to run
 * @param argc number of argument for this func
 * @param argv arguments for this func
 */
void sys_run(void *func, uint8_t argc, uint32_t argv[]);


/**
 * @brief do a software reset of the system
 */
void sys_reset(void);


/**
 * @todo need a prototype for this
 */
void sys_log();


/**
 * @brief get the last error code logged by the system
 * @return error code indicating the last problem seen by the system
 */
enum SYS_ERR sys_get_error(void);


/**
 * @brief burn a few intructions
 */
void sys_nop(void);


/**
 * @brief spin for given number of ms
 * @param time number of ms to spin
 */
void sys_spin(uint32_t time);


/**
 * @brief Initialise the hal system level
 * @note call this first thing on start-up
 */
void sys_init(void);

/**
 * @brief Initializes addition handler to be called on each tick interrupt
 * @note make sure that you call this after the init process has finished
 */
void sys_set_systick_handler(SysTick_Extra_Handler_f handler);

#endif

