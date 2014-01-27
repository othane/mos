/**
 * @file spim.c
 *
 * @brief implement the spi master module for the stm32373
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */


#include <stm32f37x_conf.h>
#include "hal.h"
#include "spi.h"
#include "spim_hw.h"
#include "gpio_hw.h"
#include "dma_hw.h"


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
	uint8_t b = SPI_ReceiveData8(spim->channel);

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
		SPI_SendData8(spim->channel, b);
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
		spim_write_phase(spim);

	// handle completion callback
	if (spim->read_count == spim->len)
	{
		spim_xfer_complete complete = spim->xfer_complete;
		void *param = spim->xfer_complete_param;
		void *read_buf = spim->read_buf;
		void *write_buf = spim->write_buf;
		int16_t len = spim->len;
		SPI_Cmd(spim->channel, DISABLE);
		gpio_set_pin(spim->nss, 1);
		SPI_I2S_ITConfig(spim->channel, SPI_I2S_IT_RXNE, DISABLE);
		SPI_I2S_ITConfig(spim->channel, SPI_I2S_IT_TXE, DISABLE);
		spim_clear_io(spim);
		if (complete != NULL)
			complete(spim, 0x00, read_buf, write_buf, len, param);
	}
}


void spim_rx_dma_complete(dma_request_t *req, void *param)
{
	spim_t *spim = (spim_t *)param;
	spim_xfer_complete complete = spim->xfer_complete;
	void *spim_xfer_param = spim->xfer_complete_param;
	void *read_buf = spim->read_buf;
	void *write_buf = spim->write_buf;
	int16_t len = spim->len;
	SPI_Cmd(spim->channel, DISABLE);
	gpio_set_pin(spim->nss, 1);
	SPI_I2S_ITConfig(spim->channel, SPI_I2S_IT_RXNE, DISABLE);
	SPI_I2S_ITConfig(spim->channel, SPI_I2S_IT_TXE, DISABLE);
	spim_clear_io(spim);
	if (complete != NULL)
		complete(spim, 0x00, read_buf, write_buf, len, spim_xfer_param);
}


void spim_xfer(spim_t *spim, int addr, void *read_buf, void *write_buf, int len, spim_xfer_complete complete, void *param)
{

	sys_enter_critical_section();

	if (spim->read_buf != NULL || spim->read_count != 0 ||
	    spim->write_buf != NULL || spim->write_count != 0)
		///@todo xfer in progress already
		goto done;

	// load xfer details
	spim->read_buf = read_buf;
	spim->read_count = 0;
	spim->write_buf = write_buf;
	spim->write_count = 0;
	spim->len = len;
	spim->xfer_complete = complete;
	spim->xfer_complete_param = param;

	// flush the buffers so the xfer begins a new
	spi_flush_rx_fifo(spim->channel);
	spi_flush_tx_fifo(spim->channel, &spim->st_spi_init);

	// init the read
	if (len <= 4 || spim->rx_dma == NULL)
	{
		SPI_I2S_ITConfig(spim->channel, SPI_I2S_IT_RXNE, ENABLE);
	}
	else
	{
		spim->rx_dma_req.complete = spim_rx_dma_complete;
		spim->rx_dma_req.complete_param = spim;
		spim->rx_dma_req.dma = spim->rx_dma;
		spi_dma_cfg(SPI_DMA_DIR_RX, spim->channel, &spim->rx_dma_req, spim->read_buf, spim->len);
		SPI_I2S_DMACmd(spim->channel, SPI_I2S_DMAReq_Rx, ENABLE);
		dma_request(&spim->rx_dma_req);
	}
	
	// init the write
	SPI_Cmd(spim->channel, ENABLE);
	gpio_set_pin(spim->nss, 0);
	if (len < 4 || spim->tx_dma == NULL)
	{
		// just write 1 byte, let the isr take over the rest
		spim_write_phase(spim);
		SPI_I2S_ITConfig(spim->channel, SPI_I2S_IT_TXE, ENABLE);
	}
	else
	{
		// since we complete when the last byte is read (this obviously happens after the last byte
		// is transmitted) we don't need a complete routine for this.
		spim->tx_dma_req.complete = NULL;
		spim->tx_dma_req.complete_param = NULL;
		spim->tx_dma_req.dma = spim->tx_dma;
		spi_dma_cfg(SPI_DMA_DIR_TX, spim->channel, &spim->tx_dma_req, spim->write_buf, spim->len);
		SPI_I2S_DMACmd(spim->channel, SPI_I2S_DMAReq_Tx, ENABLE);
		dma_request(&spim->tx_dma_req);
	}

done:
	sys_leave_critical_section();
}


void spim_init(spim_t *spim)
{
	NVIC_InitTypeDef nvic_init;

	// init the spim gpio lines
	spi_gpio_init(spim->nss, spim->sck, spim->miso, spim->mosi);
	gpio_set_pin(spim->nss, 1);
	
	// init the spim clk
	spi_clk_init(spim->channel);
	
	// setup the spim isr
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	nvic_init.NVIC_IRQChannel = spim_irq(spim);
	nvic_init.NVIC_IRQChannelPreemptionPriority = 1;
	nvic_init.NVIC_IRQChannelSubPriority = 0;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);
	
	// init dma if present
	if (spim->rx_dma)
		dma_init(spim->rx_dma);
	if (spim->tx_dma)
		dma_init(spim->tx_dma);

	// set up all the spi settings, isr's, etc and start the spi
	spi_init_regs(spim->channel, &spim->st_spi_init);
}


