/**
 * @file spi.h
 *
 * @brief interface to helper routines for spi master and slave
 *
 * @note this is a stm32 private interface & not intended for use outside of hal
 *
 * @author OT
 *
 * @date Jan 2014
 *
 */


#ifndef __SPI__
#define __SPI__


#include "hal.h"
#include "dma_hw.h"


void spi_gpio_init(gpio_pin_t *nss, gpio_pin_t *sck, gpio_pin_t *miso, gpio_pin_t *mosi);

void spi_clk_init(SPI_TypeDef *channel);

void spi_init_regs(SPI_TypeDef *channel, SPI_InitTypeDef *st_spi_init_in);

void spi_flush_tx_fifo(SPI_TypeDef *channel, SPI_InitTypeDef *st_spi_init);

void spi_flush_rx_fifo(SPI_TypeDef *channel);

#define SPI_DMA_DIR_RX 0
#define SPI_DMA_DIR_TX 1
void spi_dma_cfg(int dir, SPI_TypeDef *channel, dma_request_t *dma_req, void *buf, int len);

#endif
