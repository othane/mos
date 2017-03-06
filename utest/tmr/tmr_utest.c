/**
 * @file tmr_utest.c
 *
 * @brief unit test the tmr hal module
 *
 * This test is designed to check the tmr hal module.
 *
 * @author OT
 *
 * @date Feb 2016
 *
 */


#include <stdbool.h>
#include <string.h>
#include <hal.h>

uint32_t ticks = 0;
uint32_t updates = 0;
void tmr_update(tmr_t *tmr, void *param)
{
	tmr_stop(tmr);
	ticks = sys_get_tick();
	updates++;
}

void init(void)
{
	sys_init();
	tmr_init(&tmr_dev);
	tmr_set_update_cb(&tmr_dev, tmr_update, (void *)1);
}


int main(void)
{
	init();
	tmr_start(&tmr_dev);

	while (1)
	{
		uint32_t now = sys_get_tick();
		if (now % 10000 == 0 && !tmr_running(&tmr_dev))
			tmr_start(&tmr_dev);
	}

	return 0;
}

