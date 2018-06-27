/**
 * @file fmc.c
 *
 * @brief implement the flexible memory controller module for the stm32f4
 *
 * @author OT
 *
 * @date June 2018
 *
 */


#include <stm32f4xx_conf.h>
#include "hal.h"
#include "gpio_hw.h"
#include "fmc_hw.h"


static void init_gpio_pin_array(gpio_pin_t **pins, int len)
{
	int pi;
	for (pi = 0; pi < len; pi++)
	{
		if (!pins[pi])
			continue;
		gpio_init_pin(pins[pi]);
	}
}


static void fmc_sdram_cmd(uint8_t bank, uint32_t cmd, uint32_t autorefresh, uint32_t modereg)
{
	uint32_t sdcmr;

	// set the target bank
	switch (bank)
	{
		case 0:
			sdcmr = FMC_SDCMR_CTB1;
			break;
		case 1:
			sdcmr = FMC_SDCMR_CTB2;
			break;
		default:
			sdcmr = FMC_SDCMR_CTB1 | FMC_SDCMR_CTB2; // both
	}

	// set the auto-reload bits
	sdcmr |= ((autorefresh - 1) & 0x0f) << 5;

	// set the mode register
	sdcmr |= (modereg & 0x1fff) << 9;

	// add the cmd
	sdcmr |= (cmd & 0x7);

	// wait for the controller to be free and issue command
	while (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY);
	FMC_Bank5_6->SDCMR = sdcmr;
}


#define FORMAT_TIMING_REG(x,bit0) ((((uint32_t)x > 0) && ((uint32_t)x <= 16)? x - 1: ((uint32_t)x == 0)? 0: 15) << bit0)
void fmc_sdram_init(fmc_sdram_t *sdram)
{
	uint32_t sdcr, sdtr;
	uint32_t sdcr0, sdtr0;

	// enable the fmc clk
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);

	// init the gpio's
	gpio_init_pin(sdram->clk);
	init_gpio_pin_array(sdram->clk_en, 2);
	init_gpio_pin_array(sdram->cs, 2);

	init_gpio_pin_array(sdram->addr, 13);
	init_gpio_pin_array(sdram->bank_addr, 2);
	gpio_init_pin(sdram->row_addr_strobe);
	gpio_init_pin(sdram->col_addr_strobe);

	init_gpio_pin_array(sdram->data, 32);

	gpio_init_pin(sdram->write_enable);
	init_gpio_pin_array(sdram->byte_mask, 4);

	// setup the config registers (1)
	sdcr = FMC_Bank5_6->SDCR[sdram->bank];
	sdcr &= ~0x7fff; // reset all the setup
	sdcr = sdram->read_pipe_delay | sdram->read_burst | sdram->clk_period | \
			FMC_Write_Protection_Enable | sdram->cas_latency | sdram->internal_bank | \
			sdram->data_width | sdram->row_bits | sdram->col_bits;
	if (sdram->bank)
	{
		// because the SDCR2 RPIPE, RBURST, and SDCLK are don't care
		// and refer to the SDRC1 bits for both banks we must set these
		// here if we are dealing with bank 2
		//uint32_t sdcr0;
		sdcr0 = FMC_Bank5_6->SDCR[0];
		sdcr0 &= ~0x7c00; // this will override RPIPE, RBURST, SDCLK for both banks
		sdcr0 = sdram->read_pipe_delay | sdram->read_burst | sdram->clk_period;
		FMC_Bank5_6->SDCR[0] = sdcr0;
	}
	FMC_Bank5_6->SDCR[sdram->bank] = sdcr;

	// init timing registers (2)
	sdtr = FMC_Bank5_6->SDTR[sdram->bank];
	sdtr &= ~0xfffffff; // reset all the timing
	sdtr |= FORMAT_TIMING_REG(sdram->trcd, 24) | FORMAT_TIMING_REG(sdram->trp, 20) | \
			FORMAT_TIMING_REG(sdram->twr, 16) | FORMAT_TIMING_REG(sdram->trc, 12) | \
			FORMAT_TIMING_REG(sdram->tras, 8) | FORMAT_TIMING_REG(sdram->txsr, 4) | \
			FORMAT_TIMING_REG(sdram->tmrd, 0);
	if (sdram->bank)
	{
		// because the SDTR2 TRP (row_pre_charge_delay), TRC (trc)
		// are don't care and refer to the SDTR1 bits for both banks we must
		// set these here if we are dealing with bank 2
		//uint32_t sdtr0;
		sdtr0 = FMC_Bank5_6->SDTR[0];
		sdtr0 &= ~0xf0f000; // this will override TRP, and TRC for both banks
		sdtr0 |= FORMAT_TIMING_REG(sdram->trp, 20) | FORMAT_TIMING_REG(sdram->trc, 12);
		FMC_Bank5_6->SDTR[0] = sdtr0;
	}
	FMC_Bank5_6->SDTR[sdram->bank] = sdtr;

	// enable the fmc clk (3)
	fmc_sdram_cmd(sdram->bank, FMC_Command_Mode_CLK_Enabled, 1, 0);

	// wait delay time for the sdram to init (4)
	sys_spin(sdram->power_on_delay);

	// precharge all (5)
	fmc_sdram_cmd(sdram->bank, FMC_Command_Mode_PALL, 1, 0);

	// configure the auto refresh cycles (6)
	fmc_sdram_cmd(sdram->bank, FMC_Command_Mode_AutoRefresh, sdram->auto_refresh_cycles, 0);

	// configure the MRD field (7)
	fmc_sdram_cmd(sdram->bank, FMC_Command_Mode_LoadMode, 1, sdram->mdr);

	// refresh count (8)
	FMC_Bank5_6->SDRTR |= (sdram->refresh_rate & 0x1fff) << 1;

	// set write protect and we are done
	sdcr &= ~(FMC_Write_Protection_Enable); // clear bits
	sdcr |= sdram->write_protect;
	FMC_Bank5_6->SDCR[sdram->bank] = sdcr;
}

