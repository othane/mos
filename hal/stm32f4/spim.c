/**
 * @file spim.c
 *
 * @brief implement the spi master module for the stm32f4
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */


#include <stm32f4xx_conf.h>
#include "hal.h"
#include "spi.h"
#include "spim_hw.h"
#include "gpio_hw.h"
#include "dma_hw.h"


// set the address/nss lines
static void set_addr(spim_t *spim, uint16_t addr)
{
	gpio_pin_t **nss;

	// do this addressing as quickly as possible so it looks
	// continuous (some chips might get accidentally addressed
	// very quickly like this, but the alternative it to use
	// ports instead of pins which is less flexible)
	sys_enter_critical_section();
	for (nss = spim->nss; *nss != NULL; addr >>= 1, nss++)
	{
		if (addr & 0x01)
			gpio_set_pin(*nss, 0);
		else
			gpio_set_pin(*nss, 1);
	}
	sys_leave_critical_section();
}


// look up the irq channel for this spi master and save a look up for this object when the isr happens
static spim_t *spim_irq_list[3] = {NULL,};  ///< just store the spim handle so we can get it in the irq (then hw.c is more free form)
static uint8_t spim_irq(spim_t *spim)
{
	switch ((uint32_t)spim->channel)
	{
		case (uint32_t)SPI1:
			spim_irq_list[0] = spim;
			return SPI1_IRQn;
		case (uint32_t)SPI2:
			spim_irq_list[1] = spim;
			return SPI2_IRQn;
		case (uint32_t)SPI3:
			spim_irq_list[2] = spim;
			return SPI3_IRQn;
		default:
			///@todo error
			return 0x00;
	}
}


void spim_read_phase(spim_t *spim)
{
	uint8_t b = SPI_ReceiveData(spim->channel);

	// put the new byte into the read buffer
	if (spim->read_count < spim->len)
	{
		if (spim->read_buf != NULL)
			spim->read_buf[spim->read_count] = b;
		spim->read_count++;
	}
}


bool spim_write_phase(spim_t *spim)
{
	uint8_t b;

	if (spim->write_count < spim->len)
	{
		if (spim->write_buf == NULL)
			b = 0x00;	// if null buffer transmit a dummy byte so the reads still occur
		else
			b = spim->write_buf[spim->write_count];
		SPI_SendData(spim->channel, b);
		spim->write_count++;
		return true;
	}
	return false;
}


void spim_clear_io(spim_t *spim)
{
	spim->xfer_complete = NULL;
	spim->xfer_complete_param = NULL;
	spim->read_buf = NULL;
	spim->write_buf = NULL;
	spim->read_count = 0;
	spim->write_count = 0;
	spim->len = 0;
}

void spim_irq_handler(int n)
{
	spim_t *spim = spim_irq_list[n];

	// sanity check that spim should run
	if (spim == NULL)
		return;

	//@todo check for errors (what errors can the master have really ?)
	
	// read phase (read the Rx register)
	if (SPI_I2S_GetITStatus(spim->channel, SPI_I2S_IT_RXNE) == SET)
		spim_read_phase(spim);
	
	// write phase (reload the Tx register if it is empty and the isr is enabled)
	if (SPI_I2S_GetITStatus(spim->channel, SPI_I2S_IT_TXE) == SET)
	{
		if (!spim_write_phase(spim))
		{
			// All data sent, disable the TXE interrupt
			SPI_I2S_ITConfig(spim->channel, SPI_I2S_IT_TXE, DISABLE);
		}
	}

	// handle completion callback
	if (spim->read_count == spim->len)
	{
		spim_xfer_complete complete = spim->xfer_complete;
		void *param = spim->xfer_complete_param;
		void *read_buf = spim->read_buf;
		void *write_buf = spim->write_buf;
		int16_t len = spim->len;
		SPI_Cmd(spim->channel, DISABLE);
		set_addr(spim, spim->idle_address); // go to idle bus state
		SPI_I2S_ITConfig(spim->channel, SPI_I2S_IT_RXNE, DISABLE);
		SPI_I2S_ITConfig(spim->channel, SPI_I2S_IT_TXE, DISABLE);
		spim_clear_io(spim);
		if (complete != NULL)
			complete(spim, spim->addr, read_buf, write_buf, len, param);
	}
}

void spim_dma_complete(spim_t *spim)
{
	spim_xfer_complete complete = spim->xfer_complete;
	void *spim_xfer_param = spim->xfer_complete_param;
	void *read_buf = spim->read_buf;
	void *write_buf = spim->write_buf;
	int16_t len = spim->len;
	SPI_Cmd(spim->channel, DISABLE);
	set_addr(spim, spim->idle_address); // go to idle bus state
	SPI_I2S_ITConfig(spim->channel, SPI_I2S_IT_RXNE, DISABLE);
	SPI_I2S_ITConfig(spim->channel, SPI_I2S_IT_TXE, DISABLE);
	spim_clear_io(spim);
	if (complete != NULL)
		complete(spim, spim->addr, read_buf, write_buf, len, spim_xfer_param);
}

void spim_rx_dma_complete(dma_request_t *req, void *param)
{
	spim_t *spim = (spim_t *)param;

	spim->rx_completed = true;
	if (spim->tx_completed)
		spim_dma_complete(spim);
}

void spim_tx_dma_complete(dma_request_t *req, void *param)
{
	spim_t *spim = (spim_t *)param;

	spim->tx_completed = true;
	if (spim->rx_completed)
		spim_dma_complete(spim);
}

static const struct prescalers_t {
	uint16_t scale;
	uint16_t reg;
} prescalers[] = {
	{.scale = 2, .reg = SPI_BaudRatePrescaler_2},
	{.scale = 4, .reg = SPI_BaudRatePrescaler_4},
	{.scale = 8, .reg = SPI_BaudRatePrescaler_8},
	{.scale = 16, .reg = SPI_BaudRatePrescaler_16},
	{.scale = 32, .reg = SPI_BaudRatePrescaler_32},
	{.scale = 64, .reg = SPI_BaudRatePrescaler_64},
	{.scale = 128, .reg = SPI_BaudRatePrescaler_128},
	{.scale = 256, .reg = SPI_BaudRatePrescaler_256},
};

bool spim_xfer(spim_t *spim, spim_xfer_opts *opts, uint16_t addr, void *read_buf, void *write_buf, int len, spim_xfer_complete complete, void *param)
{
	float fclk = spi_get_clk_speed(spim->channel);
	int k;
	bool success = true;

	// update speed from opts if needed (do this outside critical section)
	if (opts->speed)
	{
		for (k = 0; k < sizeof(prescalers)/sizeof(struct prescalers_t); k++)
		{
			if ((fclk / prescalers[k].scale) <= opts->speed)
				goto success;
		}
		k--; // no solution so just use slowest possible speed
success:
		opts->st_opts.SPI_BaudRatePrescaler = prescalers[k].reg;
		opts->speed = 0; // done, we don't need to wast time calculating this again
	}

	sys_enter_critical_section();

	if (spim->read_buf != NULL || spim->read_count != 0 ||
		spim->write_buf != NULL || spim->write_count != 0)
	{
		// A transfer is already in progress
		success = false;
		goto done;
	}

	// load xfer details
	spim->addr = addr;
	spim->read_buf = read_buf;
	spim->read_count = 0;
	spim->write_buf = write_buf;
	spim->write_count = 0;
	spim->len = len;
	spim->xfer_complete = complete;
	spim->xfer_complete_param = param;

	// flush the buffers so the xfer begins a new
	spi_flush_rx_fifo(spim->channel);
	spi_flush_tx_fifo(spim->channel, &opts->st_opts);

	// init the read
	if (len == 1 || spim->rx_dma == NULL)
	{
		SPI_I2S_ITConfig(spim->channel, SPI_I2S_IT_RXNE, ENABLE);
	}
	else
	{
		spim->rx_dma_req.complete = spim_rx_dma_complete;
		spim->rx_dma_req.complete_param = spim;
		spim->rx_dma_req.dma = spim->rx_dma;
		spim->rx_completed = false;
		spi_dma_cfg(SPI_DMA_DIR_RX, spim->channel, &spim->rx_dma_req, spim->read_buf, spim->len);
		dma_request(&spim->rx_dma_req);
		SPI_I2S_DMACmd(spim->channel, SPI_I2S_DMAReq_Rx, ENABLE);
	}
	
	// init the write
	SPI_Cmd(spim->channel, ENABLE);
	set_addr(spim, spim->addr);
	if (len == 1 || spim->tx_dma == NULL)
	{
		// just write 1 byte, let the isr take over the rest
		spim_write_phase(spim);
		SPI_I2S_ITConfig(spim->channel, SPI_I2S_IT_TXE, ENABLE);
	}
	else
	{
		spim->tx_dma_req.complete = spim_tx_dma_complete;
		spim->tx_dma_req.complete_param = spim;
		spim->tx_dma_req.dma = spim->tx_dma;
		spim->tx_completed = false;
		spi_dma_cfg(SPI_DMA_DIR_TX, spim->channel, &spim->tx_dma_req, spim->write_buf, spim->len);
		SPI_I2S_DMACmd(spim->channel, SPI_I2S_DMAReq_Tx, ENABLE);
		dma_request(&spim->tx_dma_req);
	}

done:
	sys_leave_critical_section();
	return success;
}


void spim_init(spim_t *spim)
{
	NVIC_InitTypeDef nvic_init;
	gpio_pin_t **nss;

	// init the spim gpio lines (we will do the nss ourselves as this is an array)
	spi_gpio_init(NULL, spim->sck, spim->miso, spim->mosi);
	for (nss = spim->nss; *nss != NULL; nss++)
		gpio_init_pin(*nss);
	set_addr(spim, spim->idle_address); // go to idle bus state
	
	// init the spim clk
	spi_clk_init(spim->channel);
	
	// setup the spim isr
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	nvic_init.NVIC_IRQChannel = spim_irq(spim);
	nvic_init.NVIC_IRQChannelPreemptionPriority = spim->preemption_priority;
	nvic_init.NVIC_IRQChannelSubPriority = 0;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);
	
	// init dma if present
	if (spim->rx_dma)
		dma_init(spim->rx_dma);
	if (spim->tx_dma)
		dma_init(spim->tx_dma);
}


