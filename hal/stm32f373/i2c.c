/**
* @file i2c.c
*
* @brief implementation of the universal asynchronous receiver transmitter (i2c bus)
*
* @author CC
*
* @date Aug 2017
*
*/

#include <stm32f37x_conf.h>
#include "hal.h"
#include "gpio_hw.h"
#include "dma_hw.h"
#include "i2c_hw.h"

static i2c_t *i2c_irq_list[2] = {NULL,};  ///< just store the i2c handle so we can get it in the irq (then hw.c is more free form)
static uint8_t i2c_irq(i2c_t *i2c)
{
	switch ((uint32_t)i2c->channel)
	{
		case (uint32_t)I2C1:
			i2c_irq_list[0] = i2c;
			return I2C1_EV_IRQn;
		case (uint32_t)I2C2:
			i2c_irq_list[1] = i2c;
			return I2C2_EV_IRQn;
		default:
			///@todo error
			return 0x00;
	}
}

void i2c_clear_read(i2c_t *i2c)
{
	// disable the write isr
	if (i2c->tx_dma)
	{
		dma_cancel(i2c->tx_dma);
		I2C_DMACmd(i2c->channel, I2C_DMAReq_Tx, DISABLE);
	}
	I2C_ITConfig(i2c->channel,I2C_IT_RXNE, DISABLE);
	I2C_ITConfig(i2c->channel,I2C_IT_TIMEOUT,DISABLE);
	// clear the buffers for next read
	i2c->read_buf = NULL;
	i2c->read_buf_len = 0;
	i2c->read_count = 0;
	i2c->read_complete_cb = NULL;
	i2c->read_complete_param = NULL;
}

void i2c_clear_write(i2c_t *i2c)
{
	// disable the write isr
	if (i2c->tx_dma)
	{
		dma_cancel(i2c->tx_dma);
		I2C_DMACmd(i2c->channel, I2C_DMAReq_Tx, DISABLE);
	}
	I2C_ITConfig(i2c->channel,I2C_IT_TXIS, DISABLE);
	I2C_ITConfig(i2c->channel,I2C_IT_TIMEOUT,DISABLE);
	// clear the buffers for next write
	i2c->write_buf = NULL;
	i2c->write_buf_len = 0;
	i2c->write_count = 0;
	i2c->write_complete_cb = NULL;
	i2c->write_complete_param = NULL;
}

static void i2c_rx_dma_complete(dma_request_t *req, void *param)
{
	i2c_t *i2c = (i2c_t *)param;
	void *buf = i2c->read_buf;
	int16_t len = i2c->read_buf_len;
	i2c_read_complete_cb read_complete_cb = i2c->read_complete_cb;
	void *read_complete_param = i2c->read_complete_param;

	i2c_clear_read(i2c);
	if (read_complete_cb != NULL)
		read_complete_cb(i2c, buf, len, read_complete_param);
}

static void i2c_tx_dma_complete(dma_request_t *req, void *param)
{
	i2c_t *i2c = (i2c_t *)param;
	void *buf = i2c->write_buf;
	int16_t len = i2c->write_buf_len;
	i2c_write_complete_cb write_complete_cb = i2c->write_complete_cb;
	void *write_complete_param = i2c->write_complete_param;

	i2c_clear_write(i2c);
	if (write_complete_cb != NULL)
		write_complete_cb(i2c, buf, len, write_complete_param);
}

static void i2c_irq_handler(i2c_t *i2c)
{
	void *read_buf = i2c->read_buf;
	int16_t read_count;
	i2c_read_complete_cb read_complete_cb = NULL;
	void *read_complete_param = i2c->read_complete_param;

	void *write_buf = i2c->write_buf;
	int16_t write_count;
	i2c_write_complete_cb write_complete_cb = NULL;
	void *write_complete_param = i2c->write_complete_param;
	// sanity check that we setup this interrupt
	if (i2c == NULL)
		return;
	// if the receive buffer is full copy it to read_buf
	if (I2C_GetITStatus(i2c->channel,I2C_IT_RXNE) &&
		i2c->read_buf !=NULL && i2c->read_count < i2c->read_buf_len)
	{
		uint8_t rx = I2C_ReceiveData(i2c->channel);
		((uint8_t *)i2c->read_buf)[i2c->read_count++] = rx;
		
		// if the read_buf is full then read is complete
		if (i2c->read_count == i2c->read_buf_len)
		{
			read_count = i2c->read_count;
			read_complete_cb = i2c->read_complete_cb;
			i2c_clear_read(i2c);
		}
	}
	// i2c time out interrupt
	if (I2C_GetITStatus(i2c->channel,I2C_IT_TIMEOUT))
	{
		// just do simple clear for next commnunication
		i2c_clear_read(i2c);
		i2c_clear_write(i2c);
	}
	// isr transfer data to slave
	if (I2C_GetITStatus(i2c->channel,I2C_IT_TXIS) &&
			i2c->write_buf != NULL && i2c->write_count < i2c->write_buf_len)
	{
		uint8_t tx;
		tx = ((uint8_t *)i2c->write_buf)[i2c->write_count++];
		if (i2c->write_count == i2c->write_buf_len)
			I2C_ITConfig(i2c->channel,I2C_IT_TC,ENABLE);
		I2C_SendData(i2c->channel,tx);
	}
	// if the transmitter completed the last byte then the write is done
	if (I2C_GetITStatus(i2c->channel, I2C_IT_TC) &&
		i2c->write_buf != NULL && i2c->write_count == i2c->write_buf_len)
	{
		I2C_ITConfig(i2c->channel, I2C_IT_TC, DISABLE);
		write_count = i2c->write_count;
		write_complete_cb = i2c->write_complete_cb;
		i2c_clear_write(i2c);
	}
	// run deferred read/write callbacks
	if (write_complete_cb != NULL)
		write_complete_cb(i2c, write_buf, write_count, write_complete_param);
	if (read_complete_cb != NULL)
		read_complete_cb(i2c, read_buf, read_count, read_complete_param);
}

void I2C1_EV_IRQHandler(void)
{
	i2c_irq_handler(i2c_irq_list[0]);
}

void I2C2_EV_IRQHandler(void)
{
	i2c_irq_handler(i2c_irq_list[1]);
}

#define I2C_DMA_DIR_RX 0
#define I2C_DMA_DIR_TX 1
static uint32_t __NULL;
static void i2c_dma_cfg(i2c_t *i2c, int dir, dma_request_t *dma_req, void *buf, uint16_t len)
{
	DMA_InitTypeDef *i2c_cfg = &dma_req->st_dma_init;
	I2C_TypeDef *channel = i2c->channel;
	i2c_cfg->DMA_PeripheralBaseAddr = dir? (uint32_t)&channel->TXDR: (uint32_t)&channel->RXDR;
	i2c_cfg->DMA_MemoryBaseAddr = (uint32_t)buf;
	i2c_cfg->DMA_MemoryInc = DMA_MemoryInc_Enable;
	
	if(buf==NULL)
	{
		i2c_cfg->DMA_MemoryBaseAddr = (uint32_t)&__NULL;
		i2c_cfg->DMA_MemoryInc = DMA_MemoryInc_Disable;
	}
	i2c_cfg->DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	i2c_cfg->DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	i2c_cfg->DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	i2c_cfg->DMA_Mode = DMA_Mode_Normal;
	i2c_cfg->DMA_Priority = DMA_Priority_VeryHigh;
	i2c_cfg->DMA_M2M = DMA_M2M_Disable;
	i2c_cfg->DMA_BufferSize = len;
	i2c_cfg->DMA_DIR = dir ? DMA_DIR_PeripheralDST: DMA_DIR_PeripheralSRC;
}

int i2c_read(i2c_t *i2c, uint8_t device_address, void *buf, uint16_t len, i2c_read_complete_cb cb, void *param)
{   
	// sanity checks
	if (len < 1)
		///@todo invalid input parameters
		return -1;

	sys_enter_critical_section();   // lock while changing things so an isr does not find a half setup read

	uint32_t timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(i2c->channel, I2C_ISR_BUSY) != RESET)
	{
		if((timeout--) == 0) 
			goto done;
	}

	if (i2c->read_buf != NULL || i2c->read_count != 0)
		///@todo read in progress already
		goto done;

	//load the read info
	i2c->read_buf = buf;
	i2c->read_buf_len = len;
	i2c->read_count = 0;
	i2c->read_complete_cb = cb;
	i2c->read_complete_param = param;    
	// enable rx and interrupt to kick off the read
	I2C_TransferHandling(i2c->channel, device_address, len, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);  
	/* Wait until TXIS flag is set */
	timeout = I2C_LONG_TIMEOUT;

	while(I2C_GetFlagStatus(i2c->channel, I2C_ISR_RXNE) == RESET)
	{
		if((timeout--) == 0)
			goto done;
	}

	if (i2c->rx_dma)
	{
		// dma receiver 
		i2c->rx_dma_req.complete = i2c_rx_dma_complete;
		i2c->rx_dma_req.complete_param = i2c;
		i2c->rx_dma_req.dma = i2c->rx_dma;
		i2c_dma_cfg(i2c, I2C_DMA_DIR_RX, &i2c->rx_dma_req, i2c->read_buf, i2c->read_buf_len);
		I2C_DMACmd(i2c->channel, I2C_DMAReq_Rx, ENABLE);
		dma_request(&i2c->rx_dma_req);
	}
	else
	{
		// interrupt receiver
		I2C_ITConfig(i2c->channel, I2C_IT_RXI, ENABLE);
	}
done:
	sys_leave_critical_section();
	return 0;
}

int i2c_read_count(i2c_t *i2c)
{
	if(i2c->read_buf_len == 0)
		return -1;
	if (i2c->rx_dma)
		return i2c->read_buf_len - dma_remaining(&i2c->rx_dma_req);
	else
		return i2c->read_count;
}

void i2c_cancel_read(i2c_t *i2c)
{
	sys_enter_critical_section();
	i2c_clear_read(i2c);
	sys_leave_critical_section();
}

int i2c_write(i2c_t *i2c, uint8_t device_address, void *buf, uint16_t len, i2c_write_complete_cb cb, void *param)
{
	// sanity checks
	if (len < 1)
		///@todo invalid input parameters
		return -1;

	sys_enter_critical_section();   // lock while changing things so an isr does not find a half setup write

	uint32_t timeout = I2C_LONG_TIMEOUT;
	while(I2C_GetFlagStatus(i2c->channel, I2C_ISR_BUSY) != RESET)
	{
		if((timeout--) == 0)
			goto done;
	}

	if (i2c->write_buf != NULL || i2c->write_count != 0)
		///@todo write in progress already
		goto done; 
	// load the write info
	i2c->write_buf = buf;
	i2c->write_buf_len = len;
	i2c->write_count = 0;
	i2c->write_complete_cb = cb;
	i2c->write_complete_param = param;
	/*send slave address*/
	I2C_TransferHandling(i2c->channel, device_address, len, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
	timeout = I2C_LONG_TIMEOUT;

	while(I2C_GetFlagStatus(i2c->channel, I2C_ISR_TXIS) == RESET)
	{
		if((timeout--) == 0) 
			goto done;
	}

	if (i2c->tx_dma)
	{
		// dma transfer
		i2c->tx_dma_req.complete = i2c_tx_dma_complete;
		i2c->tx_dma_req.complete_param = i2c;
		i2c->tx_dma_req.dma = i2c->tx_dma;
		i2c_dma_cfg(i2c, I2C_DMA_DIR_TX, &i2c->tx_dma_req, i2c->write_buf, i2c->write_buf_len);
		I2C_DMACmd(i2c->channel, I2C_DMAReq_Tx, ENABLE);
		dma_request(&i2c->tx_dma_req);
	}    
	else
	{
		// interrupt mode 
		I2C_ITConfig(i2c->channel,I2C_IT_TXI, ENABLE);
	}
done:
	sys_leave_critical_section();
	return 0;
}

int i2c_write_count(i2c_t *i2c)
{
	if (i2c->write_buf_len == 0)
		// no write in progress
		return -1;
	if (i2c->tx_dma)
		return i2c->write_buf_len - dma_remaining(&i2c->tx_dma_req);
	else
		return i2c->write_count;  
}

void i2c_cancel_write(i2c_t *i2c)
{
	sys_enter_critical_section();
	i2c_clear_write(i2c);
	sys_leave_critical_section();
}

void i2c_init(i2c_t *i2c)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;
	/*gpio setting*/
	gpio_init_pin(i2c->scl);
	gpio_init_pin(i2c->sda);
	/*reset i2c setting*/
	I2C_DeInit(i2c->channel);
	/*initial i2c clock*/
	switch((uint32_t)i2c->channel)
	{
		case (uint32_t)I2C1:
			/*Periph clock enable*/
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
			/*Configure the i2c1 clock source. The clock is derived from the sysclk*/
			RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);
			/*clock enable*/
			RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
			break;
		case (uint32_t)I2C2:
			/*Periph clock enable*/
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
			/*Configure the i2c2 clock source. The clock is derived from the sysclk*/
			RCC_I2CCLKConfig(RCC_I2C2CLK_SYSCLK);
			/*clock enable*/
			RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
			break;
	}
	/* i2c isr */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = i2c_irq(i2c);
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/*init dma if use*/
	if (i2c->rx_dma)
	{
		dma_init(i2c->rx_dma);
	}
	if (i2c->tx_dma)
	{
		dma_init(i2c->tx_dma);
	}
	/*configure i2c*/
	I2C_InitStructure.I2C_Mode = i2c->cfg.I2C_Mode;
	I2C_InitStructure.I2C_AnalogFilter = i2c->cfg.I2C_AnalogFilter;
	I2C_InitStructure.I2C_DigitalFilter = i2c->cfg.I2C_DigitalFilter;
	I2C_InitStructure.I2C_OwnAddress1 = i2c->cfg.I2C_OwnAddress1;
	I2C_InitStructure.I2C_Ack = i2c->cfg.I2C_Ack;
	I2C_InitStructure.I2C_AcknowledgedAddress = i2c->cfg.I2C_AcknowledgedAddress;
	I2C_InitStructure.I2C_Timing = i2c->cfg.I2C_Timing;
	/*enable i2c after configure*/
	I2C_Init(i2c->channel, &I2C_InitStructure);
	I2C_Cmd(i2c->channel,ENABLE);
	I2C_AcknowledgeConfig(i2c->channel,ENABLE);
	I2C_Init(i2c->channel, &I2C_InitStructure);
	I2C_Cmd(i2c->channel,ENABLE);
}
