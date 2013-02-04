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
}


static void spis_clear_write(spis_t *spis)
{
	// reset write buffers, next time the isr runs it will have no where to write too and give up
	spis->write_buf = NULL;
	spis->write_buf_len = 0;
	spis->write_count = 0;
	spis->write_complete_cb = NULL;
	spis->write_complete_param = NULL;
	SPI_I2S_ITConfig(spis->channel, SPI_I2S_IT_TXE, DISABLE);
}


// outside world interface to cancel a write op
void spis_cancel_write(spis_t *spis)
{
	spis_clear_write(spis);
}


// look up the irq channel for this spi slave and save a look up for this object when the isr happens
static spis_t *spis_irq_list[3] = {NULL,};	///< just store the spis handle so we can get it in the irq (then hw.c is more free form)
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


void spis_init(spis_t *spis)
{
	SPI_InitTypeDef st_spi_init;
	NVIC_InitTypeDef nvic_init;
	
	// clear the buffers
	spis_clear_read(spis);
	spis_clear_write(spis);
	
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
	
	// init the spis gpio lines
	gpio_init_pin(spis->nss); ///@todo hook nss line to both edge interrupt so we can call the transaction start/stop events
	gpio_init_pin(spis->sck);
	gpio_init_pin(spis->miso);
	gpio_init_pin(spis->mosi);


	// setup the spis isr
	nvic_init.NVIC_IRQChannel = spis_irq(spis);
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0;
	nvic_init.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&nvic_init);

	// init the spis itself
	SPI_I2S_DeInit(spis->channel);
	SPI_StructInit(&st_spi_init);
	st_spi_init.SPI_CPHA = SPI_CPHA_2Edge; 			// clock phase
	st_spi_init.SPI_CPOL = SPI_CPOL_Low; 			// clock polarity
	st_spi_init.SPI_DataSize = SPI_DataSize_8b;
	st_spi_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	st_spi_init.SPI_FirstBit = SPI_FirstBit_MSB;
	st_spi_init.SPI_Mode = SPI_Mode_Slave;
	st_spi_init.SPI_NSS = SPI_NSS_Hard;
	SPI_Init(spis->channel, &st_spi_init);

	// enable the spis isrs
	SPI_I2S_ITConfig(spis->channel, SPI_I2S_IT_RXNE, ENABLE);
	
	// start spis device
	SPI_Cmd(spis->channel, ENABLE);
	SPI_Cmd(SPI3,ENABLE);
}


void spis_set_trans_start(spis_t *spis, spis_trans_start_stop start)
{
	///@todo unstub this when etxi is going 
}


void spis_set_trans_stop(spis_t *spis, spis_trans_start_stop stop)
{
	///@todo unstub this when etxi is going 
}


static void spis_irq_handler(spis_t *spis)
{
	// sanity check that spis should run
	if (spis == NULL)
		// this spis is not setup
		return;
	
	// read phase (read the Rx register)
	if (SPI_I2S_GetITStatus(spis->channel, SPI_I2S_IT_RXNE) == SET)
	{
	  	uint8_t rx_byte = SPI_I2S_ReceiveData(spis->channel);
		
		// Rx buffer has data, do we have anywhere and enough space to store it
		if ((spis->read_buf != NULL) && (spis->read_count < spis->read_buf_len) && (spis->read_buf_len > 0))
			// buffer the last Rx byte
			spis->read_buf[spis->read_count] = rx_byte;
		
		if (spis->read_count < spis->read_buf_len)
			// more bytes to read (keep counting)
			spis->read_count++;
		
		if ((spis->read_count == spis->read_buf_len) && (spis->read_buf_len > 0))
		{
			// we have read all the bytes so clean up read and run the read complete callback		  
			spis_read_complete cb = spis->read_complete_cb;
			void *buf = spis->read_buf;
			uint16_t len = spis->read_count;
			void *param = spis->read_complete_param;
			spis_clear_read(spis);
			if (cb != NULL)
				cb(spis, (void *)buf, len, (void *)param);
		}		
	}
		
	// write phase (reload the Tx register if it is empty and the isr is enabled)
	else if ((SPI_I2S_GetITStatus(spis->channel, SPI_I2S_IT_TXE) == SET) && (spis->channel->CR2 & SPI_CR2_TXEIE))
	{
	  	volatile uint8_t tx_byte = 0xCC;	// transmit a fixed pattern if we have nothing to send
		
		// do we have anything to transmit
		if ((spis->write_buf != NULL) && (spis->write_count < spis->write_buf_len) && (spis->write_buf_len > 0))
			tx_byte = spis->write_buf[spis->write_count];
		
		// send the data
		SPI_I2S_SendData(spis->channel, tx_byte);
				
		if (spis->write_count < spis->write_buf_len)
			// more bytes to write, keep counting		  
			spis->write_count++;
		
		if ((spis->write_count == spis->write_buf_len) && (spis->write_buf_len > 0))
		{
			// if we have written all the bytes in the buffer then run the clean up
			// write and call write complete callback.
			spis_write_complete cb = spis->write_complete_cb;
			void *buf = spis->write_buf;
			uint16_t len = spis->write_count;
			void *param = spis->write_complete_param;
			spis_clear_write(spis);
			if (cb != NULL)
				cb(spis, (void *)buf, len, (void *)param);
		}		
	}
	
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
	///@todo more sanity checks
	if (buf == NULL || len < 1)
		///@todo invalid input parameters
		goto error;
	sys_enter_critical_section();	// lock while changing things so an isr does not find a half setup read
	if (spis->read_buf != NULL || spis->read_count != 0)
		///@todo read in progress already
		goto error;
	
	// load the read info once we leave the critical section the isr will populate the read buffer for us
	spis->read_buf = buf;
	spis->read_buf_len = len;
	spis->read_count = 0;
	spis->read_complete_cb = cb;
	spis->read_complete_param = param;

error:
	sys_leave_critical_section();
	return;
}


void spis_write(spis_t *spis, void *buf, uint16_t len, spis_write_complete cb, void *param)
{
	///@todo more sanity checks
	if (buf == NULL || len < 1)
		///@todo invalid input parameters
		goto error;
	sys_enter_critical_section();	// lock while changing things so an isr does not find a half setup read
	if (spis->write_buf != NULL || spis->write_count != 0)
		///@todo write in progress already
		goto error;

	// load the write info
	spis->write_buf = buf;
	spis->write_buf_len = len;
	spis->write_count = 0;
	spis->write_complete_cb = cb;
	spis->write_complete_param = param;
	
	// kick off the write by sending the first byte the isr will handle sending the remaining bytes
	if (SPI_I2S_GetITStatus(spis->channel, SPI_I2S_IT_TXE) == SET)
	{
		SPI_I2S_SendData(spis->channel, spis->write_buf[0]);
		spis->write_count++;
	}
	
	// enable the send to complete via isr
	SPI_I2S_ITConfig(spis->channel, SPI_I2S_IT_TXE, ENABLE);	

error:
	sys_leave_critical_section();
	return;
}


