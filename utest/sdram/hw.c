/**
 * @file hw.c
 * 
 * @brief implements the default hw available on the stm32f429 disco board
 *
 * @see hw.h for instructions to override the defaults
 *
 * @author OT
 *
 * @date Jan 2018
 *
 */

#include <hal.h>


#if defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F446xx) || defined(STM32F469_479xx)

	#include <stm32f4xx_conf.h>

	//
	// spi
	//
	#include <gpio_hw.h>
	gpio_pin_t gpio_spi1_nss    = {GPIOA, {GPIO_Pin_4, GPIO_Mode_AF, GPIO_Speed_50MHz}, 5};
	gpio_pin_t gpio_spi1_sck    = {GPIOA, {GPIO_Pin_5,  GPIO_Mode_AF, GPIO_Speed_50MHz}, 5};
	gpio_pin_t gpio_spi1_miso   = {GPIOA, {GPIO_Pin_6,  GPIO_Mode_AF, GPIO_Speed_50MHz}, 5};
	gpio_pin_t gpio_spi1_mosi   = {GPIOA, {GPIO_Pin_7,  GPIO_Mode_AF, GPIO_Speed_50MHz}, 5};

	#include <dma_hw.h>
	dma_t spis_rx_dma =
	{
		.stream = DMA2_Stream0,
		.channel = DMA_Channel_3,
	};
	dma_t spis_tx_dma =
	{
		.stream = DMA2_Stream3,
		.channel = DMA_Channel_3,
	};

	#include <spis_hw.h>
	spis_t spis_dev =
	{
		.channel = SPI1,   // channel
		.st_spi_init = {SPI_Direction_2Lines_FullDuplex, SPI_Mode_Slave, SPI_DataSize_8b, SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_NSS_Hard, SPI_BaudRatePrescaler_2, SPI_FirstBit_MSB, 0},
		.nss = &gpio_spi1_nss, // select line gpio
		.sck = &gpio_spi1_sck, // clk line gpio
		.miso = &gpio_spi1_miso, // miso gpio
		.mosi = &gpio_spi1_mosi, // mosi gpio

		.rx_dma = &spis_rx_dma,
		.tx_dma = &spis_tx_dma,
	};

	//
	// sd ram
	//
	#include <gpio_hw.h>
	#include <fmc_hw.h>
	gpio_pin_t sdclk_pg8 = { .port = GPIOG, .cfg = {.GPIO_Pin = GPIO_Pin_8, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t sdcke1_pb5 = { .port = GPIOB, .cfg = {.GPIO_Pin = GPIO_Pin_5, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t sdne1_pb6 = { .port = GPIOB, .cfg = {.GPIO_Pin = GPIO_Pin_6, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};

	gpio_pin_t a0_pf0 = {.port = GPIOF, .cfg = {.GPIO_Pin = GPIO_Pin_0, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t a1_pf1 = {.port = GPIOF, .cfg = {.GPIO_Pin = GPIO_Pin_1, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t a2_pf2 = {.port = GPIOF, .cfg = {.GPIO_Pin = GPIO_Pin_2, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t a3_pf3 = {.port = GPIOF, .cfg = {.GPIO_Pin = GPIO_Pin_3, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t a4_pf4 = {.port = GPIOF, .cfg = {.GPIO_Pin = GPIO_Pin_4, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t a5_pf5 = {.port = GPIOF, .cfg = {.GPIO_Pin = GPIO_Pin_5, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t a6_pf12 = {.port = GPIOF, .cfg = {.GPIO_Pin = GPIO_Pin_12, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t a7_pf13 = {.port = GPIOF, .cfg = {.GPIO_Pin = GPIO_Pin_13, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t a8_pf14 = {.port = GPIOF, .cfg = {.GPIO_Pin = GPIO_Pin_14, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t a9_pf15 = {.port = GPIOF, .cfg = {.GPIO_Pin = GPIO_Pin_15, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t a10_pg0 = {.port = GPIOG, .cfg = {.GPIO_Pin = GPIO_Pin_0, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t a11_pg1 = {.port = GPIOG, .cfg = {.GPIO_Pin = GPIO_Pin_1, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};

	gpio_pin_t d0_pd14 = {.port = GPIOD, .cfg = {.GPIO_Pin = GPIO_Pin_14, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t d1_pd15 = {.port = GPIOD, .cfg = {.GPIO_Pin = GPIO_Pin_15, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t d2_pd0 = {.port = GPIOD, .cfg = {.GPIO_Pin = GPIO_Pin_0, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t d3_pd1 = {.port = GPIOD, .cfg = {.GPIO_Pin = GPIO_Pin_1, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t d4_pe7 = {.port = GPIOE, .cfg = {.GPIO_Pin = GPIO_Pin_7, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t d5_pe8 = {.port = GPIOE, .cfg = {.GPIO_Pin = GPIO_Pin_8, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t d6_pe9 = {.port = GPIOE, .cfg = {.GPIO_Pin = GPIO_Pin_9, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t d7_pe10 = {.port = GPIOE, .cfg = {.GPIO_Pin = GPIO_Pin_10, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t d8_pe11 = {.port = GPIOE, .cfg = {.GPIO_Pin = GPIO_Pin_11, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t d9_pe12 = {.port = GPIOE, .cfg = {.GPIO_Pin = GPIO_Pin_12, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t d10_pe13 = {.port = GPIOE, .cfg = {.GPIO_Pin = GPIO_Pin_13, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t d11_pe14 = {.port = GPIOE, .cfg = {.GPIO_Pin = GPIO_Pin_14, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t d12_pe15 = {.port = GPIOE, .cfg = {.GPIO_Pin = GPIO_Pin_15, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t d13_pd8 = {.port = GPIOD, .cfg = {.GPIO_Pin = GPIO_Pin_8, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t d14_pd9 = {.port = GPIOD, .cfg = {.GPIO_Pin = GPIO_Pin_9, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t d15_pd10 = {.port = GPIOD, .cfg = {.GPIO_Pin = GPIO_Pin_10, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};

	gpio_pin_t ba0_pg4 = {.port = GPIOG, .cfg = {.GPIO_Pin = GPIO_Pin_4, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t ba1_pg5 = {.port = GPIOG, .cfg = {.GPIO_Pin = GPIO_Pin_5, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};

	gpio_pin_t sdnras_pf11 = {.port = GPIOF, .cfg = {.GPIO_Pin = GPIO_Pin_11, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t sdncas_pg15 = {.port = GPIOG, .cfg = {.GPIO_Pin = GPIO_Pin_15, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t sdnwe_pc0 = {.port = GPIOC, .cfg = {.GPIO_Pin = GPIO_Pin_0, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};

	gpio_pin_t nbl0_pe0 = {.port = GPIOE, .cfg = {.GPIO_Pin = GPIO_Pin_0, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};
	gpio_pin_t nbl1_pe1 = {.port = GPIOE, .cfg = {.GPIO_Pin = GPIO_Pin_1, .GPIO_Mode = GPIO_Mode_AF, .GPIO_Speed=GPIO_Speed_50MHz, .GPIO_PuPd = GPIO_PuPd_NOPULL}, .af = 12};


	// see IS42S16400J data sheet (page 19)
	#define IS42S16400J_BURST_LEN2 (0x001)
	#define IS42S16400J_BURST_TYPE_SEQUENTIAL (0x000)
	#define IS42S16400J_CAS_LATENCY_3 (0x030)
	#define IS42S16400J_OPERATING_MODE_STD (0x000)
	#define IS42S16400J_WRITE_BURST_MODE_SINGLE (0x200)

	fmc_sdram_t sdram = {
		// gpio
		.clk = &sdclk_pg8,
		.clk_en = {&sdcke1_pb5,},
		.cs = {&sdne1_pb6,},
		.addr = {
			&a0_pf0, &a1_pf1, &a2_pf2, &a3_pf3, &a4_pf4, &a5_pf5, &a6_pf12,
			&a7_pf13, &a8_pf14, &a9_pf15, &a10_pg0, &a11_pg1,
		},
		.bank_addr = {&ba0_pg4, &ba1_pg5,},
		.row_addr_strobe = &sdnras_pf11,
		.col_addr_strobe = &sdncas_pg15,
		.data = {
			&d0_pd14, &d1_pd15, &d2_pd0, &d3_pd1, &d4_pe7, &d5_pe8, &d6_pe9,
			&d7_pe10, &d8_pe11, &d9_pe12, &d10_pe13, &d11_pe14, &d12_pe15,
			&d13_pd8, &d14_pd9, &d15_pd10,
		},
		.write_enable = &sdnwe_pc0,
		.byte_mask = {&nbl0_pe0, &nbl1_pe1,},

		// sdram config
		.bank = 1,
		.read_pipe_delay = FMC_ReadPipe_Delay_1,
		.read_burst = FMC_Read_Burst_Disable,
		.clk_period = FMC_SDClock_Period_2, // HCLK=168MHz, SDCLK < 143MHz(@cas_latency=3), SDCLK=143MHz/2=84Mhz
		.write_protect = FMC_Write_Protection_Disable,
		.cas_latency = FMC_CAS_Latency_3, // so we can go as fast as possible
		.internal_bank = FMC_InternalBank_Number_4,
		.data_width = FMC_SDMemory_Width_16b,
		.row_bits = FMC_RowBits_Number_12b,
		.col_bits = FMC_ColumnBits_Number_8b,
		.auto_refresh_cycles = 4,

		// timings (HCLK=168MHz, SDCLK=HCLK/2=84MHz -> 12ns) (see IS42S16400J datasheet p16 for details)
		.trcd = 2, // > 15ns * 84MHz = 1.26cyc -> 2cyc
		.trp = 2, // > 15ns * 84MHz = 1.26cyc -> 2cyc
		.twr = 2, // > 2CLK -> 2cyc
		.trc = 7, // > 63ns * 84MHz = 5.3cyc -> 6cyc
		.tras = 4, // > 42ns * 84MHz = 3.5cyc -> 4cyc && < 100us * 84MHz = 8400cyc
		.txsr = 7, // > 70ns * 84MHz = 5.9ns -> 6cyc
		.tmrd = 2, // 2cyc

		.power_on_delay = 100, // supposedly 1cyc, but this is done once on startup so just wait a long time to be sure
		.mdr = \
			IS42S16400J_BURST_LEN2 | \
			IS42S16400J_BURST_TYPE_SEQUENTIAL | \
			IS42S16400J_CAS_LATENCY_3 | \
			IS42S16400J_OPERATING_MODE_STD | \
			IS42S16400J_WRITE_BURST_MODE_SINGLE,

		.refresh_rate = 1292, // < rate 64ms/4096rows = 15.6us -> (15.6us* 84MHz) - 20 = 1292.5cyc -> 1292cyc
	};

#else

	#error "sdram not supported on unknown target"

#endif
