/**
 * @file hw.c
 *
 * @brief spis hw file for the stm32f3
 *
 * @see hw.h for instructions to override the defaults
 *
 * @author CC
 *
 * @date Aug 2017
 *
 */

#include <hal.h>

#if defined STM32F37X
	#include <stm32f37x_conf.h>
	#include <gpio_hw.h>
	#include <dma_hw.h>
	#include <i2c_hw.h>
    // i2c module
    dma_t i2c_rx_dma = {
        .channel = DMA1_Channel5,
        .preemption_priority = 0,
    };
    dma_t i2c_tx_dma = {
        .channel = DMA1_Channel4,
        .preemption_priority = 0,
    };
    gpio_pin_t i2c_scl_pin = {
        .port = GPIOA,
        .cfg = {
            .GPIO_Pin = GPIO_Pin_9, 
            .GPIO_Mode = GPIO_Mode_AF,
            .GPIO_Speed = GPIO_Speed_2MHz, 
            .GPIO_OType = GPIO_OType_OD,
            .GPIO_PuPd = GPIO_PuPd_NOPULL
            },
            .af = 4,
    };    
    gpio_pin_t i2c_sda_pin = {
        .port = GPIOA,
        .cfg = {
            .GPIO_Pin = GPIO_Pin_10, 
            .GPIO_Mode = GPIO_Mode_AF,
            .GPIO_Speed = GPIO_Speed_2MHz,
            .GPIO_OType = GPIO_OType_OD,
            .GPIO_PuPd = GPIO_PuPd_NOPULL
             },
            .af = 4,
    };
    i2c_t i2c_dev = {
        .channel = I2C2, //channel
        .cfg = {
            .I2C_Mode = I2C_Mode_I2C,
            .I2C_AnalogFilter = I2C_AnalogFilter_Enable,
            .I2C_DigitalFilter = 0x00,
            .I2C_OwnAddress1 = 0x00,
            .I2C_Ack = I2C_Ack_Enable,
            .I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit,
            .I2C_Timing = 0xC062121F
        },
        .scl = &i2c_scl_pin,
        .sda = &i2c_sda_pin,
        .rx_dma = &i2c_rx_dma,
        .tx_dma = &i2c_tx_dma,
    };
    /*switch blade i2c alarm*/
    gpio_pin_t i2c_alarm = {
        .port = GPIOC,
        .cfg = {
            .GPIO_Pin = GPIO_Pin_6, 
            .GPIO_Mode = GPIO_Mode_IN,
            .GPIO_Speed = GPIO_Speed_50MHz, 
            .GPIO_OType = GPIO_OType_PP},
    };
    /*switch blade i2c busy state*/
    gpio_pin_t i2c_busy = {
        .port = GPIOC,
        .cfg = {
            .GPIO_Pin = GPIO_Pin_6, 
            .GPIO_Mode = GPIO_Mode_IN,
            .GPIO_Speed = GPIO_Speed_50MHz, 
            .GPIO_OType = GPIO_OType_PP},
    };
    /*switch blade i2c reset*/
    gpio_pin_t i2c_reset = {
        .port = GPIOA,
        .cfg = {
            .GPIO_Pin = GPIO_Pin_7, 
            .GPIO_Mode = GPIO_Mode_OUT,
            .GPIO_Speed = GPIO_Speed_50MHz, 
            .GPIO_OType = GPIO_OType_PP},
    };

#endif
