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

#include <stm32f10x_conf.h>
#include "hal.h"
#include "spis_hw.h"
#include "gpio_hw.h"


static void spis_clear_read(spis_t *spis)
{
	// reset read buffers, next time the isr runs it will have no where to read from and give up
	spis->read_buf = NULL;
	spis->read_buf_len = 0;
	spis->read_count = 0;
	spis->read_complete_cb = NULL;
	spis->read_complete_param = NULL;
}


// outside world interface to cancel a read op
void spis_cancel_read(spis_t *spis)
{
	spis_clear_read(spis);

	// clear the Rx buffer so the next read will start fresh
	while (SPI_I2S_GetFlagStatus(spis->channel, SPI_I2S_FLAG_RXNE) == SET)
		SPI_I2S_ReceiveData(spis->channel);
}


static void spis_clear_write(spis_t *spis)
{
	// reset write buffers, next time the isr runs it will have no where to write too and give up
	spis->write_buf = NULL;
	spis->write_buf_len = 0;
	spis->write_count = 0;
	spis->write_complete_cb = NULL;
	spis->write_complete_param = NULL;
}


// outside world interface to cancel a write op
void spis_cancel_write(spis_t *spis)
{
	spis_clear_write(spis);
}


// look up the irq channel for this spi slave and save a look up for this object when the isr happens
static spis_t *spis_irq_list[3] = {NULL,};  ///< just store the spis handle so we can get it in the irq (then hw.c is more free form)
uint8_t spis_irq(spis_t *spis)
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
	SPI_InitTypeDef st_spi_init;
	NVIC_InitTypeDef nvic_init;

	// clear the buffers
	spis_clear_read(spis);
	spis_clear_write(spis);

	// init the spis gpio lines
	// we seem to need to init a [so I choose these since they need to be inited anyway] gpio 
	// before the spi_clk or else irqs dont seem to work and possibly the whole spis doesnt
	gpio_init_pin(spis->nss);
	gpio_init_pin(spis->sck);
	gpio_init_pin(spis->miso);
	gpio_init_pin(spis->mosi);

	// init the spis clk
	switch ((uint32_t)spis->channel)
	{
		case (uint32_t)SPI1:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
			break;
		case (uint32_t)SPI2:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
			break;
		case (uint32_t)SPI3:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
			AFIO->MAPR |= 0x02000000; // disable JTAG and enable SWD
			break;

		default:
			///@todo error out here
			return;
	}
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

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

	// init the spis itself
	SPI_I2S_DeInit(spis->channel);
	SPI_StructInit(&st_spi_init);
	st_spi_init.SPI_CPHA = SPI_CPHA_1Edge;          // clock phase
	st_spi_init.SPI_CPOL = SPI_CPOL_High;           // clock polarity
	st_spi_init.SPI_DataSize = SPI_DataSize_16b;
	st_spi_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	st_spi_init.SPI_FirstBit = SPI_FirstBit_MSB;
	st_spi_init.SPI_Mode = SPI_Mode_Slave;
	st_spi_init.SPI_NSS = SPI_NSS_Hard;
	SPI_Init(spis->channel, &st_spi_init);

	// enable the spis isrs
	SPI_I2S_ITConfig(spis->channel, SPI_I2S_IT_RXNE, ENABLE);
	SPI_I2S_ITConfig(spis->channel, SPI_I2S_IT_TXE, ENABLE);
	SPI_I2S_ITConfig(spis->channel, SPI_I2S_IT_ERR, ENABLE);

	// start spis device
	SPI_Cmd(spis->channel, ENABLE);
}


// read the bytes from the rx buf move the buffers on and possibly call the completion event
static spis_read_complete spis_read_phase(spis_t *spis, void **buf, uint16_t *len, void **param)
{
	union
	{
		uint8_t b[2];
		uint16_t w;
	} rx_byte;
	rx_byte.w = SPI_I2S_ReceiveData(spis->channel);

	// Rx buffer has data, do we have anywhere and enough space to store it
	if ((spis->read_buf != NULL) && (spis->read_count < spis->read_buf_len) && (spis->read_buf_len > 0))
	{
		// buffer the first Rx byte
		spis->read_buf[spis->read_count] = rx_byte.b[1];

		// if the buffer is long enough get the second byte
		if (spis->read_count+1 < spis->read_buf_len)
			spis->read_buf[spis->read_count + 1] = rx_byte.b[0];
	}

	// increment the number of bytes read, do it twice as we possibly read 2 bytes
	if (spis->read_count < spis->read_buf_len)
		spis->read_count++;
	if (spis->read_count < spis->read_buf_len)
		spis->read_count++;
	
	// if we have read all the bytes so clean up read and run the read complete callback
	if ((spis->read_count == spis->read_buf_len) && (spis->read_buf_len > 0))
	{
		spis_read_complete read_cb = spis->read_complete_cb;
		*buf = spis->read_buf;
		*len = spis->read_count;
		*param = spis->read_complete_param;
		spis_clear_read(spis);
		return read_cb;

		spis_read_complete cb = spis->read_complete_cb;
		void *buf = spis->read_buf;
		uint16_t len = spis->read_count;
		void *param = spis->read_complete_param;
		spis_clear_read(spis);
		return cb;
		if (cb != NULL)
			cb(spis, (void *)buf, len, (void *)param);
	}
	return NULL;
}

// write the bytes to the tx buf move the buffers on and possibly call the completion event
static spis_write_complete spis_write_phase(spis_t *spis, void **buf, uint16_t *len, void **param)
{
	volatile union
	{
		uint8_t b[2];
		uint16_t w;
	} tx_byte;
	tx_byte.w = 0xAACC; // transmit a fixed pattern if we have nothing to send

	// do we have anything to transmit
	if ((spis->write_buf != NULL) && (spis->write_count < spis->write_buf_len) && (spis->write_buf_len > 0))
	{
		tx_byte.b[1] = spis->write_buf[spis->write_count];

		// if we have enough bytes to send the get the next byte too
		if ((spis->write_count+1) < spis->write_buf_len)
			tx_byte.b[0] = spis->write_buf[spis->write_count + 1];
	}

	// send the data
	SPI_I2S_SendData(spis->channel, tx_byte.w);

	// increment the number of bytes written, do it twice as we possibly write 2 bytes
	if (spis->write_count < spis->write_buf_len)
		spis->write_count++;
	if (spis->write_count < spis->write_buf_len)
		spis->write_count++;
	
	// if we have written all the bytes in the buffer then run the clean up
	if ((spis->write_count == spis->write_buf_len) && (spis->write_buf_len > 0))
	{
		spis_write_complete write_cb = spis->write_complete_cb;
		*buf = spis->write_buf;
		*len = spis->write_count;
		*param = spis->write_complete_param;
		spis_clear_write(spis);
		return write_cb;

		spis_write_complete cb = spis->write_complete_cb;
		void *buf = spis->write_buf;
		uint16_t len = spis->write_count;
		void *param = spis->write_complete_param;
		spis_clear_write(spis);
		if (cb != NULL)
			cb(spis, (void *)buf, len, (void *)param);
	}
	return NULL;
}


static void spis_irq_handler(spis_t *spis)
{
	spis_read_complete read_cb = NULL;
	spis_write_complete write_cb = NULL;
	void *read_cb_buf, *write_cb_buf;
	uint16_t read_cb_len, write_cb_len;
	void *read_cb_param, *write_cb_param;

	// sanity check that spis should run
	if (spis == NULL)
		// this spis is not setup
		return;

	// check for errors and report them
	///@todo maybe crc
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


// handle the spi1 isr (only handles slave atm)
void SPI1_IRQHandler(void)
{
	spis_irq_handler(spis_irq_list[0]);
}


// handle the spi1 isr (only handles slave atm)
void SPI2_IRQHandler(void)
{
	spis_irq_handler(spis_irq_list[1]);
}


// handle the spi1 isr (only handles slave atm)
void SPI3_IRQHandler(void)
{
	spis_irq_handler(spis_irq_list[2]);
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

	// read any already pending data (note call spis_cancel_read first to start fresh)
	if (SPI_I2S_GetFlagStatus(spis->channel, SPI_I2S_FLAG_RXNE) == SET)
	{
		read_cb = spis_read_phase(spis, &read_cb_buf, &read_cb_len, &read_cb_param);
		if (read_cb != NULL)
			read_cb(spis, (void *)read_cb_buf, read_cb_len, (void *)read_cb_param);
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
		goto error;

	sys_enter_critical_section();   // lock while changing things so an isr does not find a half setup read

	if (spis->write_buf != NULL || spis->write_count != 0)
		///@todo write in progress already
		goto error;

	// load the write info
	spis->write_buf = buf;
	spis->write_buf_len = len;
	spis->write_count = 0;
	spis->write_complete_cb = cb;
	spis->write_complete_param = param;

	// kick off the write by sending the first word the isr will handle sending the remaining bytes
	write_cb = spis_write_phase(spis, &write_cb_buf, &write_cb_len, &write_cb_param);
	if (write_cb != NULL)
		write_cb(spis, (void *)write_cb_buf, write_cb_len, (void *)write_cb_param);
	
	SPI_I2S_ITConfig(spis->channel, SPI_I2S_IT_TXE, ENABLE);
error:
	sys_leave_critical_section();
	return;
}


bool spis_selected(spis_t *spis)
{
	return (!gpio_get_pin(spis->nss));
}


