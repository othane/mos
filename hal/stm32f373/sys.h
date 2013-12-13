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
 * @brief get the number of 1ms intervals since boot
 * @note there is not attempt to deal with rollovers in this function
 * @return the number of 1ms ticks that have occurred since boot time
 */
uint32_t sys_get_tick(void);


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

#endif

