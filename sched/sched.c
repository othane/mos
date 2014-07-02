/**
 * @file sched.c
 *
 * @brief implement the mos scheduler
 *
 * @author OT
 *
 * @date June 2014
 *
 */


#include <string.h>
#include <hal.h>
#include "sched.h"

struct task_info_t
{
	task_id_t task_id;
	uint32_t time;
	int priority;
	void *cb;
	uint8_t argc;
	uint32_t argv[SCHED_MAX_TASK_PARAMS];
	struct task_info_t *next, *prev;	// double link list for speed
};

static struct task_info_t task_list[SCHED_MAX_TASKS];
static struct task_info_t *task_list_head = NULL;
static task_id_t task_id = 0; // global task id index (incremented so no tasks should have the same id)


static struct task_info_t * alloc_task()
{
	int t;

	for (t = 0; t < SCHED_MAX_TASKS; t++)
	{
		if (task_list[t].cb == NULL)
			// a null callback indicates this is a free task
			return &task_list[t];
	}

	// task not found
	return NULL;
}

static void free_task(struct task_info_t *task)
{
	memset(task, 0, sizeof(*task));
}

// insert task between a and b
static void insert_task(struct task_info_t *a, struct task_info_t *b, struct task_info_t *task)
{
	if (a)
		a->next = task;
	if (b)
		b->prev = task;
	task->prev = a;
	task->next = b;
	if (!task_list_head || b == task_list_head)
		task_list_head = task;
}

task_id_t sched_add_task(uint32_t time, uint8_t priority, void *callback, uint8_t argc, ...)
{
	struct task_info_t *new_task, *prev_task, *task;
	uint8_t k;
	va_list ap;
	task_id_t ret = -1;

	// sanity checks on task
	if (callback == NULL)
		return -1;
	if (argc < 0 || argc > SCHED_MAX_TASK_PARAMS)
		return -1;

	// protect task_list with critical section
	sys_enter_critical_section();

	// alloc[find] a new free task to use
	new_task = alloc_task();
	if (!new_task)
		goto done;
	
	// populate task info
	memset(new_task, 0, sizeof(*new_task));
	new_task->task_id = task_id++; // set this to the next global id and inc the global id
	new_task->time = time;
	new_task->priority = priority;
	new_task->cb = callback;
	new_task->argc = argc;
	va_start(ap, argc);
	for (k=0; k < argc; k++)
		new_task->argv[k] = va_arg(ap, uint32_t);
	va_end(ap);

	// find where this task should go in the task
	// link list (it is ordered by time where the 
	// first task is the next to run)
	for (prev_task = NULL, task = task_list_head; task; prev_task = task, task = task->next)
	{
		if (sys_tick_diff(time, task->time) > 0)
			// task runs later than the new_task so we want to prepend new_task
			break;
	}
	
	// insert this task into the task_list
	if (!task_list_head)
		// only task in the list
		insert_task(prev_task, task, new_task); // NULL, NULL, new_task
	else if (!task)
		// last task in the list
		insert_task(prev_task, task, new_task); // prev_task, NULL, new_task
	else
		// in the middle of the list somewhere
		insert_task(prev_task, task, new_task); // prev_task, task, new_task
	ret = new_task->task_id;
	
done:
	sys_leave_critical_section();

	return ret;
}

int sched_rm_task(task_id_t task)
{	
	struct task_info_t *t, *prev, *next;
	int ret = 0;

	sys_enter_critical_section();

	// find the task in the task list
	for (t = task_list_head; t; t = t->next)
	{
		if (t->task_id == task)
			// task id found in list
			break;
	}
	if (t->task_id != task)
		goto done; // task not in list

	// remove the task from the list
	prev = t->prev;
	next = t->next;
	if (prev)
		prev->next = t->next;
	if (next)
		next->prev = t->prev;
	if (t == task_list_head)
		// if we are removing the head then we need to update it
		task_list_head = next;
	
	// free the task
	free_task(t);
	ret = 1;

done:
	sys_leave_critical_section();
	return ret;
}

// find the next task to run, ie highest priority task out of all the late tasks
struct task_info_t * get_next_late_task(void)
{
	struct task_info_t *t = NULL, *next = NULL;
	uint32_t now;

	sys_enter_critical_section();
	now = sys_get_tick();
	for (t = task_list_head; t; t = t->next)
	{
		if (sys_tick_diff(now, t->time) > 0)
			// since task list is sorted by time all tasks after this
			// are not yet late so we are done
			break;

		if (next == NULL || t->priority > next->priority)
			// this task is late and has the highest priority so far
			next = t;
	}
	sys_leave_critical_section();

	return next;
}

int sched_run_tasks(int empty)
{
	int n = 0;

	while (1)
	{
		struct task_info_t task, *t;

		// find the next highest priority late task if there is one
		t = get_next_late_task();
		if (!t)
			return n; // no late tasks
		
		// save this task and remove it from the list
		task = *t;
		sched_rm_task(t->task_id);
		
		// run the task
		sys_run(task.cb, task.argc, task.argv);
		n++;

		// option to run just one task at a time, or until no late tasks remain
		if (!empty)
			return n;
	}
}

void sched_init(void)
{
	memset(task_list, 0, sizeof(task_list));
}

