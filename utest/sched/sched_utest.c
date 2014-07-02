/**
 * @file sched_utest.c
 *
 * @brief unit test the sched module
 *
 * This test is designed to check the scheduler module adds/removes/runs
 * tasks on time and in the correct order
 *
 * @author OT
 *
 * @date July 2014
 *
 */


#include <mos.h>

void task_periodic(uint32_t time, uint32_t ts)
{
	gpio_toggle_pin(&tp1);

	time += ts;
	sched_add_task(time, 1, task_periodic, 2, time, ts);
}

void task3(void)
{
	sys_nop();
}


void task2(void *v, int i)
{
	sys_nop();
}


void task2_hp(void *v, int i)
{
	sys_nop();
}


void task1(uint8_t a, uint16_t b, uint32_t c, int d)
{
	sys_nop();
}


void init()
{
	sys_init();
	gpio_init_pin(&tp1);
	sched_init();
}


int main(void)
{
	task_id_t t1,t2,t3;
	uint32_t start;
	unused int ret;

	init();

	// task list is empty lets add some tasks in an awkward order and then
	// remove them
	start = sys_get_tick();
	t3 = sched_add_task(start + 14, 1, task3, 0);
	t1 = sched_add_task(start + 10, 1, task1, 4, 0xfe, 0xfffe, 0xfffffffe, -1);
	t2 = sched_add_task(start + 12, 1, task2, 2, NULL, 1);
	sched_rm_task(t3);
	sched_rm_task(t1);
	sched_rm_task(t2);

	// task list should be empty start so ret should equal 0 below
	sys_spin(50);
	ret = sched_run_tasks(1);
	
	// add some tasks back in to run in a new awkward order
	start = sys_get_tick();
	t3 = sched_add_task(start + 14, 1, task3, 0);
	t2 = sched_add_task(start + 12, 1, task2, 2, NULL, 2);
	t2 = sched_add_task(start + 12, 2, task2_hp, 2, NULL, 2);
	t1 = sched_add_task(start + 10, 1, task1, 4, 0xfe, 0xfffe, 0xfffffffe, -1);

	// run these tasks (they should all run in under 100ms)
	while (sys_tick_diff(start, sys_get_tick()) < 100)
		ret = sched_run_tasks(0);

	// try to remove tasks that have already run (should fail to remove)
	ret = sched_rm_task(t3);
	ret = sched_rm_task(t1);
	ret = sched_rm_task(t2);

	// run periodic task forever (check for 2ms square wave on scope)
	start = sys_get_tick();
	sched_add_task(start + 10, 1, task_periodic, 2, start, 1);
	while (1)
		ret = sched_run_tasks(1);
	
	return 0;
}

