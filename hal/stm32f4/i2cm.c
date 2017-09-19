/**
 * @file i2cm.c
 *
 * @brief implement the i2c master module for the stm32f4
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */


#include <stm32f4xx_conf.h>
#include "hal.h"
#include "i2c.h"
#include "i2cm_hw.h"
#include "gpio_hw.h"
#include "dma_hw.h"


static uint8_t i2cm_clk(i2cm_t *i2cm)
{
	switch ((int)i2cm->channel)
	{
		case I2C1:
			return RCC_APB1Periph_I2C1;
		case I2C2:
			return RCC_APB1Periph_I2C2;
		default:
			///@todo error invalid I2C master
			return 0;
	}
}


static uint8_t i2cm_irq(i2cm_t *i2cm)
{
	switch ((int)i2cm->channel)
	{
		case I2C1:
			return I2C1_EV_IRQn | I2C1_ER_IRQn;
		case I2C2:
			return I2C2_EV_IRQn | I2C2_ER_IRQn;
		default:
			///@todo error invalid I2C master
			return 0;
	}
}


int i2cm_read(i2cm_t *i2cm, uint16_t addr, void *buf, int len, i2cm_read_complete_t complete, void *param)
{
}


void i2cm_init(i2cm_t *i2cm)
{
	I2C_InitTypeDef  I2C_InitStructure;
	NVIC_InitTypeDef nvic_init;

	// init the i2cm clk
	RCC_APB1PeriphClockCmd(i2cm_clk(i2cm), ENABLE);
	RCC_I2CCLKConfig(RCC_I2CxCLK_SYSCLK);

	// init the i2cm gpio lines
	gpio_init_pin(&i2cm->sda);
	gpio_init_pin(&i2cm->scl);

	// init i2c module in master mode
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_AnalogFilter = i2cm->analog_filter;
	I2C_InitStructure.I2C_DigitalFilter = i2cm->digital_filter;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = i2c->address_size;
	I2C_InitStructure.I2C_Timing = i2cm->timing; ///@todo calculate this somehow
	I2C_Init(i2cm-channel, &I2C_InitStructure);

	// setup the i2cm isrs
	nvic_init.NVIC_IRQChannel = i2cm_irq(i2cm);
	nvic_init.NVIC_IRQChannelPreemptionPriority = i2cm->preemption_priority;
	nvic_init.NVIC_IRQChannelSubPriority = 0;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);
	
	// init dma if present
	#if 0
	if (i2cm->rx_dma)
		dma_init(i2cm->rx_dma);
	if (i2cm->tx_dma)
		dma_init(i2cm->tx_dma);
	#endif

	// enable i2c master
	I2C_Cmd(i2cm->channel, ENABLE);
}

