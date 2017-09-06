/**
 * @file spi.c
 *
 * @brief helper routines for spi master and slave
 *
 * @author OT
 *
 * @date Jan 2014 
 *
 */

#include <stm32f4xx_conf.h>
#include "hal.h"
#include "gpio_hw.h"
#include "dma_hw.h"


void spi_gpio_init(gpio_pin_t *nss, gpio_pin_t *sck, gpio_pin_t *miso, gpio_pin_t *mosi)
{
	gpio_init_pin(nss);
	gpio_init_pin(sck);
	gpio_init_pin(miso);
	gpio_init_pin(mosi);
}


void spi_clk_init(SPI_TypeDef *channel)
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


float spi_get_clk_speed(SPI_TypeDef *channel)
{
	switch ((uint32_t)channel)
	{
		case (uint32_t)SPI1:
			// on apb2 bus so full speed
			return sys_clk_freq();
		case (uint32_t)SPI2:
		case (uint32_t)SPI3:
		default:
			// on apb1 bus so 1/2 speed
			return sys_clk_freq() / 2.0;
	}
}


void spi_init_regs(SPI_TypeDef *channel, SPI_InitTypeDef *st_spi_init_in)
{
	// init the spi itself
	SPI_InitTypeDef st_spi_init = *st_spi_init_in;
	st_spi_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // only full duplex mode is supported atm
	SPI_I2S_DeInit(channel);
	SPI_Init(channel, &st_spi_init);
	
	// enable the spi isrs
	SPI_I2S_ITConfig(channel, SPI_I2S_IT_ERR, ENABLE);
}


void spi_flush_tx_fifo(SPI_TypeDef *channel, SPI_InitTypeDef *st_spi_init)
{
	// unfortunately st did not create a way to flush the fifo's out
	// easily, the only way I have been able to achieve this is via a
	// APB reset, then a re-init
	SPI_I2S_DeInit(channel);

	// thanks to the re-init above we need to reset up all the spi regs again
	spi_init_regs(channel, st_spi_init);
}


void spi_flush_rx_fifo(SPI_TypeDef *channel)
{
	// the easiest/only way to clear the rx fifo is to read out 
	// all the bytes until the RXNE flag is cleared (note this 
	// will require rx thresh = QF (I think)
	while (SPI_I2S_GetFlagStatus(channel, SPI_I2S_FLAG_RXNE) == SET)
		SPI_I2S_ReceiveData(channel);
}


static uint32_t __NULL;
void spi_dma_cfg(int dir, SPI_TypeDef *channel, dma_request_t *dma_req, void *buf, int len)
{
	DMA_InitTypeDef *spi_cfg = &dma_req->st_dma_init;
	dma_t *dma = dma_req->dma;

	spi_cfg->DMA_Channel = dma->channel;
	spi_cfg->DMA_PeripheralBaseAddr = (uint32_t)&channel->DR;
	spi_cfg->DMA_Memory0BaseAddr = (uint32_t)buf;
	spi_cfg->DMA_MemoryInc = DMA_MemoryInc_Enable;
	if (buf == NULL)
	{
		// this NULL is a possible security hole as multiple io may read/write to
		// this, it is only really for dbg and a real buffer should really be used
		// at all times
		spi_cfg->DMA_Memory0BaseAddr = (uint32_t)&__NULL;
		spi_cfg->DMA_MemoryInc = DMA_MemoryInc_Disable;
	}
	spi_cfg->DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	spi_cfg->DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	spi_cfg->DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	spi_cfg->DMA_Mode = DMA_Mode_Normal;
	spi_cfg->DMA_Priority = DMA_Priority_High;
	spi_cfg->DMA_FIFOMode = DMA_FIFOMode_Disable;
	spi_cfg->DMA_MemoryBurst = DMA_MemoryBurst_Single;
	spi_cfg->DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	spi_cfg->DMA_BufferSize = len;
	spi_cfg->DMA_DIR = dir ? DMA_DIR_MemoryToPeripheral: DMA_DIR_PeripheralToMemory;
}


// dummy irq handler that is overridden if spi slave is included
weak void spis_irq_handler(int n)
{
}


// dummy irq handler that is overridden if spi master is included
weak void spim_irq_handler(int n)
{
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


