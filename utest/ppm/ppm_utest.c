/**
 * @file ppm_utest.c
 *
 * @brief unit test the ppm hal module
 *
 * This test is designed to check the ppm module with the help of an
 * oscilloscope ... running this test should show a 1KHz PPM signal with a 0
 * degrees reference and another drifting signal (drifting from 0 to 360 deg)
 *
 * @author OT
 *
 * @date Apr 2015
 *
 */


#include <hal.h>

void init(void)
{
	sys_init();
	ppm_init(&ppm_ref);
	ppm_init(&ppm_drift);
}

#define dphs (1.0 / 1000)
int main(void)
{
	float phs = 0;

	init();
	ppm_start(&ppm_ref);
	ppm_start(&ppm_drift);

	ppm_set_freq(&ppm_drift, 10000); // test overriding hw.c freq and ensure ch2 updates phs too
	while (1)
	{
		ppm_set_phs(&ppm_drift, phs);
		phs += dphs;
		while (phs > 1.0)
			phs -= 1.0;
		sys_spin(5);
	}

	return 0;
}

