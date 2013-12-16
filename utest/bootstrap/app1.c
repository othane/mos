/**
 * @file app1.c
 *
 * @brief bootstrap unit test (application 1)
 *
 * Just switch to application 2
 *
 * @author OT
 *
 * @date Dec 2013
 *
 */

#include <hal.h>

// program header for bootstrap
extern void *g_pfnVectors;
struct bootstrap_prog_header app1_program_header at_symbol(".program_header") = 
{
	.type = BOOTSTRAP_PROG_HEADER,
	.pid = 1,
	.isr_vector = &g_pfnVectors,
	.max_len = (8 * 1024),	// copied from linker script (yuk, surely it can fill this in for me somehow)
	.hw_id = BOOTSTRAP_PROG_HEADER_HW_ID,
};

int main(void)
{
	bootstrap_set_boot_pid(2);
	sys_reset();
	return 0;
}
