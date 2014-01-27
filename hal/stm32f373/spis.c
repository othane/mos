/**
 * @file spis.c
 *
 * @brief impliment the spis module for the stm32107
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */

#include <stm32f37x_conf.h>
#include "hal.h"
#include "spis_hw.h"
#include "gpio_hw.h"
#include "dma_hw.h"


static void spi_gpio_init(gpio_pin_t *nss, gpio_pin_t *sck, gpio_pin_t *miso, gpio_pin_t *mosi)
{
	gpio_init_pin(nss);
	gpio_init_pin(sck);
	gpio_init_pin(miso);
	gpio_init_pin(mosi);
}


static void spi_clk_init(SPI_TypeDef *channel)
{
	switch ((uint32_t)channel)
	{
		case (uint32_t)SPI1:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
			break;
		case (uint32_t)SPI2:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
			break;
		case (uint32_t)SPI3:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
			break;
		default:
			///@todo error out here
			return;
	}
}


static void spi_init_regs(SPI_TypeDef *channel, SPI_InitTypeDef *st_spi_init_in)
{
	// init the spi itself
	SPI_InitTypeDef st_spi_init = *st_spi_init_in;
	st_spi_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // only full duplex mode is supported atm
	SPI_I2S_DeInit(channel);
	SPI_Init(channel, &st_spi_init);
	
	// enable the spi isrs
	SPI_RxFIFOThresholdConfig(channel, SPI_RxFIFOThreshold_QF);
	SPI_I2S_ITConfig(channel, SPI_I2S_IT_ERR, ENABLE);
}


static void spi_flush_tx_fifo(SPI_TypeDef *channel, SPI_InitTypeDef *st_spi_init)
{
	// unfortunately st did not create a way to flush the fifo's out
	// easily, the only way I have been able to achieve this is via a
	// APB reset, then a re-init
	SPI_I2S_DeInit(channel);

	// thanks to the re-init above we need to reset up all the spi regs again
	spi_init_regs(channel, st_spi_init);
}


static void spi_flush_rx_fifo(SPI_TypeDef *channel)
{
	// the easiest/only way to clear the rx fifo is to read out 
	// all the bytes until the RXNE flag is cleared (note this 
	// will require rx thresh = QF (I think)
	while (SPI_I2S_GetFlagStatus(channel, SPI_I2S_FLAG_RXNE) == SET)
		SPI_I2S_ReceiveData16(channel);
}


#define DIR_RX 0
#define DIR_TX 1
static uint32_t __NULL;
static void spi_dma_cfg(int dir, SPI_TypeDef *channel, dma_request_t *dma_req, void *buf, int len)
{
	DMA_InitTypeDef *spi_cfg = &dma_req->st_dma_init;

	spi_cfg->DMA_PeripheralBaseAddr = (uint32_t)&channel->DR;
	spi_cfg->DMA_MemoryBaseAddr = (uint32_t)buf;
	if (buf == NULL)
	{
		// this NULL is a possible security hole as multiple io may read/write to
		// this, it is only really for dbg and a real buffer should really be used
		// at all times
		spi_cfg->DMA_MemoryBaseAddr = (uint32_t)&__NULL;
		spi_cfg->DMA_MemoryInc = DMA_MemoryInc_Disable;
	}
	spi_cfg->DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	spi_cfg->DMA_MemoryInc = DMA_MemoryInc_Enable;
	spi_cfg->DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	spi_cfg->DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	spi_cfg->DMA_Mode = DMA_Mode_Normal;
	spi_cfg->DMA_Priority = DMA_Priority_High;
	spi_cfg->DMA_M2M = DMA_M2M_Disable;
	spi_cfg->DMA_BufferSize = len;
	spi_cfg->DMA_DIR = dir ? DMA_DIR_PeripheralDST: DMA_DIR_PeripheralSRC;
}


// look up the irq channel for this spi master and save a look up for this object when the isr happens
static spim_t *spim_irq_list[3] = {NULL,};  ///< just store the spis handle so we can get it in the irq (then hw.c is more free form)
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

	// sanity check that spis should run
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
		spim->rx_dma_req.complete = NULL;
		spim->rx_dma_req.complete_param = NULL;
		spim->rx_dma_req.dma = spim->rx_dma;
		spi_dma_cfg(DIR_RX, spim->channel, &spim->rx_dma_req, spim->read_buf, spim->len);
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
		spim->tx_dma_req.complete = NULL;
		spim->tx_dma_req.complete_param = NULL;
		spim->tx_dma_req.dma = spim->tx_dma;
		spi_dma_cfg(DIR_TX, spim->channel, &spim->tx_dma_req, spim->write_buf, spim->len);
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


static void spis_clear_read(spis_t *spis)
{
	// reset read buffers, next time the isr runs it will have no where to read from and give up
	SPI_I2S_ITConfig(spis->channel, SPI_I2S_IT_RXNE, DISABLE);
	spis->read_buf = NULL;
	spis->read_buf_len = 0;
	spis->read_count = 0;
	spis->read_complete_cb = NULL;
	spis->read_complete_param = NULL;
	spi_flush_rx_fifo(spis->channel);
	if (spis->rx_dma)
		dma_cancel(spis->rx_dma);
}


// outside world interface to cancel a read op
void spis_flush_read(spis_t *spis)
{
	spis_clear_read(spis);
}


static void spis_clear_write(spis_t *spis, bool flush)
{
	// disable the Tx isr, and clear the buffers and fifos
	SPI_I2S_ITConfig(spis->channel, SPI_I2S_IT_TXE, DISABLE);
	spis->write_buf = NULL;
	spis->write_buf_len = 0;
	spis->write_count = 0;
	spis->write_complete_cb = NULL;
	spis->write_complete_param = NULL;
	if (spis->tx_dma)
		dma_cancel(spis->tx_dma);
	if (flush)
	{
		spi_flush_tx_fifo(spis->channel, &spis->st_spi_init);
		SPI_Cmd(spis->channel, ENABLE);
	}
}


// outside world interface to cancel a write op
void spis_flush_write(spis_t *spis)
{
	spis_clear_write(spis, true);
}


// look up the irq channel for this spi slave and save a look up for this object when the isr happens
static spis_t *spis_irq_list[3] = {NULL,};  ///< just store the spis handle so we can get it in the irq (then hw.c is more free form)
static uint8_t spis_irq(spis_t *spis)
{
	switch ((uint32_t)spis->channel)
	{
		case (uint32_t)SPI1:
			spis_irq_list[0] = spis;
			return SPI1_IRQn;
		case (uint32_t)SPI2:
			spis_irq_list[1] = spis;
			return SPI2_IRQn;
		case (uint32_t)SPI3:
			spis_irq_list[2] = spis;
			return SPI3_IRQn;
		default:
			///@todo error
			return 0x00;
	}
}


static void spis_select(gpio_pin_t *pin, void *param)
{
	// just run select callback
	spis_t *spis = (spis_t *)param;
	if (spis->select_cb != NULL)
		spis->select_cb(spis, spis->select_cb_param);
}


void spis_set_select_cb(spis_t *spis, spis_select_cb select_cb, void *param)
{
	spis->select_cb = select_cb;
	spis->select_cb_param = param;
}


static void spis_deselect(gpio_pin_t *pin, void *param)
{
	// just run deselect callback
	spis_t *spis = (spis_t *)param;
	if (spis->deselect_cb != NULL)
		spis->deselect_cb(spis, spis->deselect_cb_param);
}


void spis_set_deselect_cb(spis_t *spis, spis_deselect_cb deselect_cb, void *param)
{
	spis->deselect_cb = deselect_cb;
	spis->deselect_cb_param = param;
}


void spis_set_error_cb(spis_t *spis, spis_error_cb cb, void *param)
{
	spis->error_cb = cb;
	spis->error_cb_param = param;
}


void spis_init(spis_t *spis)
{
	NVIC_InitTypeDef nvic_init;

	// clear the buffers
	spis_clear_read(spis);
	spis_clear_write(spis, false);

	// init the spis gpio lines
	spi_gpio_init(spis->nss, spis->sck, spis->miso, spis->mosi);

	// init the spis clk
	spi_clk_init(spis->channel);

	// setup the select / deselect callbacks
	///@todo we might not want to do this for all spis devs, perhaps there should be a flag in the spis_t
	gpio_set_falling_edge_event(spis->nss, spis_select, spis);
	gpio_set_rising_edge_event(spis->nss, spis_deselect, spis);

	// setup the spis isr
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	nvic_init.NVIC_IRQChannel = spis_irq(spis);
	nvic_init.NVIC_IRQChannelPreemptionPriority = 1;
	nvic_init.NVIC_IRQChannelSubPriority = 0;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);

	// init dma if present
	if (spis->rx_dma)
		dma_init(spis->rx_dma);
	if (spis->tx_dma)
		dma_init(spis->tx_dma);

	// set up all the spi settings, isr's, etc and start the spi
	spi_init_regs(spis->channel, &spis->st_spi_init);
	SPI_Cmd(spis->channel, ENABLE);
}


// read the bytes from the rx buf move the buffers on and possibly call the completion event
static spis_read_complete spis_read_phase(spis_t *spis, void **buf, uint16_t *len, void **param)
{
	// pull the next byte out of the fifo
	uint8_t b = SPI_ReceiveData8(spis->channel);

	// put the new byte into the read buffer (always inc read_count incase we are doing a 
	// dummy read, ie read_buf == NULL but we want the cb to run)
	if (spis->read_buf != NULL && spis->read_count < spis->read_buf_len && spis->read_buf_len > 0)
		spis->read_buf[spis->read_count] = b;
	spis->read_count++;
	
	// if we are done return cb params for the irq handler to call when it has handled all the
	// other higher priority stuff
	if (spis->read_count == spis->read_buf_len && spis->read_buf_len > 0)
	{
		spis_read_complete read_cb = spis->read_complete_cb;
		*buf = spis->read_buf;
		*len = spis->read_count;
		*param = spis->read_complete_param;
		spis_clear_read(spis);
		return read_cb;
	}

	return NULL;
}

// write the bytes to the tx buf move the buffers on and possibly call the completion event
static spis_write_complete spis_write_phase(spis_t *spis, void **buf, uint16_t *len, void **param)
{
	uint8_t b = 0xaa; // send this dummy byte if we have nothing else available

	// put the new byte into the tx fifo (always inc write_count incase we are doing a 
	// dummy write, ie write_buf == NULL but we want the cb to run)
	if (spis->write_buf != NULL && spis->write_count < spis->write_buf_len && spis->write_buf_len > 0)
		b = spis->write_buf[spis->write_count];
	spis->write_count++;

	SPI_SendData8(spis->channel, b);

	if (spis->write_count == spis->write_buf_len && spis->write_buf_len > 0)
	{
		spis_write_complete write_cb = spis->write_complete_cb;
		*buf = spis->write_buf;
		*len = spis->write_count;
		*param = spis->write_complete_param;
		spis_clear_write(spis, false);
		return write_cb;
	}

	return NULL;
}


static void spis_irq_handler(int n)
{
	spis_read_complete read_cb = NULL;
	spis_write_complete write_cb = NULL;
	void *read_cb_buf, *write_cb_buf;
	uint16_t read_cb_len, write_cb_len;
	void *read_cb_param, *write_cb_param;
	spis_t *spis = spis_irq_list[n];

	// sanity check that spis should run
	if (spis == NULL)
		return;

	// check for errors and report them
	if (SPI_I2S_GetITStatus(spis->channel, SPI_I2S_IT_OVR) == SET)
	{
		if (spis->error_cb != NULL)
			spis->error_cb(spis, SPIS_ERR_OVRRUN, spis->error_cb_param);
	}
	if (SPI_I2S_GetITStatus(spis->channel, I2S_IT_UDR) == SET)
	{
		if (spis->error_cb != NULL)
			spis->error_cb(spis, SPIS_ERR_UNDRUN, spis->error_cb_param);
	}

	// read phase (read the Rx register)
	if (SPI_I2S_GetITStatus(spis->channel, SPI_I2S_IT_RXNE) == SET)
		read_cb = spis_read_phase(spis, &read_cb_buf, &read_cb_len, &read_cb_param);

	// write phase (reload the Tx register if it is empty and the isr is enabled)
	if (SPI_I2S_GetITStatus(spis->channel, SPI_I2S_IT_TXE) == SET)
		write_cb = spis_write_phase(spis, &write_cb_buf, &write_cb_len, &write_cb_param);

	// run deferred completion events, defer these so that we read new data out
	// and write new data in to the spi registers as quickly as possible
	if (read_cb != NULL)
		read_cb(spis, (void *)read_cb_buf, read_cb_len, (void *)read_cb_param);
	if (write_cb != NULL)
		write_cb(spis, (void *)write_cb_buf, write_cb_len, (void *)write_cb_param);
}


// handle the spi1 isr
void SPI1_IRQHandler(void)
{
	spis_irq_handler(0);
	spim_irq_handler(0);
}


// handle the spi1 isr
void SPI2_IRQHandler(void)
{
	spis_irq_handler(1);
	spim_irq_handler(1);
}


// handle the spi1 isr
void SPI3_IRQHandler(void)
{
	spis_irq_handler(2);
	spim_irq_handler(2);
}


static void spis_rx_dma_complete(dma_request_t *req, void *param)
{
	spis_t *spis = (spis_t *)param;
	spis_read_complete read_cb = NULL;
	void *buf;
	uint16_t len;
	void *spis_read_param;
	
	read_cb = spis->read_complete_cb;
	buf = spis->read_buf;
	spis->read_count = spis->read_buf_len;
	len = spis->read_buf_len;
	spis_read_param = spis->read_complete_param;
	spis_clear_read(spis);
	if (read_cb != NULL)
		read_cb(spis, buf, len, spis_read_param);
}


static void spis_tx_dma_complete(dma_request_t *req, void *param)
{
	spis_t *spis = (spis_t *)param;

	spis_write_complete write_cb = NULL;
	void *buf;
	uint16_t len;
	void *spis_write_param;
	
	write_cb = spis->write_complete_cb;
	buf = spis->write_buf;
	spis->write_count = spis->write_buf_len;
	len = spis->write_buf_len;
	spis_write_param = spis->write_complete_param;
	spis_clear_write(spis, false);
	if (write_cb != NULL)
		write_cb(spis, buf, len, spis_write_param);
}


void spis_read(spis_t *spis, void *buf, uint16_t len, spis_read_complete cb, void *param)
{
	spis_read_complete read_cb = NULL;
	void *read_cb_buf;
	uint16_t read_cb_len;
	void *read_cb_param;

	///@todo more sanity checks
	if (len < 1)
		///@todo invalid input parameters
		goto error;
	sys_enter_critical_section();   // lock while changing things so an isr does not find a half setup read
	if (spis->read_buf != NULL || spis->read_count != 0)
		///@todo read in progress already
		goto error;

	// load the read info once we leave the critical section the isr will populate the read buffer for us
	spis->read_buf = buf;
	spis->read_buf_len = len;
	spis->read_count = 0;
	spis->read_complete_cb = cb;
	spis->read_complete_param = param;

	if (len <= 4 || spis->rx_dma == NULL)
	{
		// read any already pending data (note call spis_cancel_read first to start fresh)
		if (SPI_I2S_GetFlagStatus(spis->channel, SPI_I2S_FLAG_RXNE) == SET)
		{
			read_cb = spis_read_phase(spis, &read_cb_buf, &read_cb_len, &read_cb_param);
			if (read_cb != NULL)
				read_cb(spis, (void *)read_cb_buf, read_cb_len, (void *)read_cb_param);
		}
		SPI_I2S_ITConfig(spis->channel, SPI_I2S_IT_RXNE, ENABLE);
	}
	else
	{
		spis->rx_dma_req.complete = spis_rx_dma_complete;
		spis->rx_dma_req.complete_param = spis;
		spis->rx_dma_req.dma = spis->rx_dma;
		spi_dma_cfg(DIR_RX, spis->channel, &spis->rx_dma_req, spis->read_buf, spis->read_buf_len);
		SPI_I2S_DMACmd(spis->channel, SPI_I2S_DMAReq_Rx, ENABLE);
		dma_request(&spis->rx_dma_req);
	}

error:
	sys_leave_critical_section();
	return;
}


void spis_write(spis_t *spis, void *buf, uint16_t len, spis_write_complete cb, void *param)
{
	spis_write_complete write_cb = NULL;
	void *write_cb_buf;
	uint16_t write_cb_len;
	void *write_cb_param;

	///@todo more sanity checks
	if (len < 1)
		///@todo invalid input parameters
		return;

	sys_enter_critical_section();   // lock while changing things so an isr does not find a half setup read

	if (spis->write_buf != NULL || spis->write_count != 0)
		///@todo write in progress already
		goto done;

	// load the write info
	spis->write_buf = buf;
	spis->write_buf_len = len;
	spis->write_count = 0;
	spis->write_complete_cb = cb;
	spis->write_complete_param = param;
	
	if (len < 4 || spis->tx_dma == NULL)
	{
		// if we are only writing 3 bytes or less or have no dma its better to do this using interrupts
		// so kick off the write by sending the first bytes the isr will handle sending the remaining bytes
		while (SPI_I2S_GetFlagStatus(spis->channel, SPI_I2S_FLAG_TXE))
		{
			write_cb = spis_write_phase(spis, &write_cb_buf, &write_cb_len, &write_cb_param);
			if (write_cb != NULL)
			{
				write_cb(spis, (void *)write_cb_buf, write_cb_len, (void *)write_cb_param);
				goto done;
			}
		}
		SPI_I2S_ITConfig(spis->channel, SPI_I2S_IT_TXE, ENABLE);
	}
	else
	{
		spis->tx_dma_req.complete = spis_tx_dma_complete;
		spis->tx_dma_req.complete_param = spis;
		spis->tx_dma_req.dma = spis->tx_dma;
		spi_dma_cfg(DIR_TX, spis->channel, &spis->tx_dma_req, spis->write_buf, spis->write_buf_len);
		SPI_I2S_DMACmd(spis->channel, SPI_I2S_DMAReq_Tx, ENABLE);
		dma_request(&spis->tx_dma_req);
	}
done:
	sys_leave_critical_section();
	return;
}

