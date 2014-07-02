/**
 * @file sched.h
 *
 * @brief interface to the mos scheduler
 *
 * @author OT
 *
 * @date June 2014
 *
 * @note: all tasks are soft tasks and are run to completion, if a task is
 * blocked by another task it will require the blocking task to complete,
 * once complete all late tasks are run in order of their priorities.
 *
 */

#include <stdarg.h>
#include <stdint.h>

#ifndef __SCHED__
#define __SCHED__

#ifndef SCHED_MAX_TASKS
#define SCHED_MAX_TASKS (8)
#endif

#ifndef SCHED_MAX_TASK_PARAMS
#define SCHED_MAX_TASK_PARAMS (4)
#endif

typedef uint32_t task_id_t;

/**
 * @brief add a task to the task queue to run at a certain time with a certain priority
 * @param time the time (according to sys_get_tick) when this task should be run
 * @param priority run late tasks in the order of this priority
 * @param callback run this callback when the task runs
 * @param argc number of arguments following this, these arguments are passed to the callback
 */
task_id_t sched_add_task(uint32_t time, uint8_t priority, void *callback, uint8_t argc, ...);

/**
 * @brief remove the specified task from the task list if it exists
 * @param task this is the task returned from sched_add_task that should be removed from the call list
 * @return if the task was found and removed return 0, else return 1
 */
int sched_rm_task(task_id_t task);

/**
 * @brief run pending tasks according to their priority
 * @param empty all the late task before returning, else just run 1 task and return
 * @return the number of tasks run
 */
int sched_run_tasks(int empty);

/**
 * @brief init this module
 */
void sched_init(void);

#endif
