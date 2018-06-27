/**
 * @file fmc.h
 *
 * @brief this contains hw definitions for configuration via hw.c only (it is not a run time interface !)
 *
 * @author OT
 *
 * @date Jan 2018
 *
 */

#ifndef __FMC_HW__
#define __FMC_HW__


#include "hal.h"
#include "gpio_hw.h"


// internal representation of a adc
struct fmc_sdram_t
{
	// gpio to the sdram chip
	gpio_pin_t *clk;
	gpio_pin_t *clk_en[2];
	gpio_pin_t *cs[2];

	gpio_pin_t *addr[13];
	gpio_pin_t *bank_addr[2];
	gpio_pin_t *row_addr_strobe;
	gpio_pin_t *col_addr_strobe;

	gpio_pin_t *data[32];

	gpio_pin_t *write_enable;
	gpio_pin_t *byte_mask[4];

	// config
	uint8_t bank;  // selects the stm32f4 bank, ie bank 1 [0] or bank 2 [1] (see FMC_SDRAM_Bank)
	uint32_t read_pipe_delay;  // see FMC_ReadPipe_Delay in HCLK cycles (note this is a global flag shared by both banks)
	uint32_t read_burst;  // see FMC_Read_Burst (note this is a global flag shared by both banks)
	uint32_t clk_period;  // see FMC_SDClock_Period (note this is a global flag shared by both banks)
	uint32_t write_protect;  // see FMC_Write_Protection
	uint32_t cas_latency;  // see FMC_CAS_Latency
	uint32_t internal_bank;  // see FMC_InternalBank_Number
	uint32_t data_width;  // see FMC_SDMemory_Data_Width
	uint32_t row_bits;  // see FMC_RowBits_Number
	uint32_t col_bits;  // see FMC_ColumnBits_Number
	uint32_t auto_refresh_cycles; // the number of auto refresh cycles for this sdram chip (1 .. 15, typical 8)

	// timing (all units 1..16 in clock cycles, see clk_period above)
	uint8_t trcd; // delay between cmd and r/w operation
	uint8_t trp; // delay between precharge cmd and next op (note this is a global flag shared by both banks)
	uint8_t twr; // delay between a write and precharge cmd (this must match the slowest sdram chip present)
	uint8_t trc; // delay between refresh cmd and active/refresh cmd (note this is a global flag shared by both banks)
	uint8_t tras; // minimum self refresh period
	uint8_t txsr; // delay from self refresh cmd to active cmd
	uint8_t tmrd; // delay between a load mode register cmd and an active refresh cmd

	uint32_t power_on_delay; // delay from the sdram datasheet from power on to talking (in ms)
	uint16_t mdr; // load this value into the mdr
	uint16_t refresh_rate; // refresh timer value loaded into the SDRTR register (in SDCLK cycles)
};

#endif
