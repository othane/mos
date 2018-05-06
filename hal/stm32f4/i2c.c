/**
 * @file i2c.c
 *
 * @brief I2C Master and Slave driver
 *
 * @author Charles Oram
 *
 * @date May 2018
 *
 */

#include <stm32f4xx_conf.h>
#include "hal.h"
#include "gpio_hw.h"
#include "i2c_hw.h"

// Driver state
typedef enum
{
    I2C_STATE_IDLE,
    I2C_STATE_BUSY_START_TX,    // Master sending start for write
    I2C_STATE_BUSY_START_RX,    // Master sending start for read
    I2C_STATE_BUSY_TX_ADDRESS,  // Master sending address, slave waiting for address, for a write
    I2C_STATE_BUSY_RX_ADDRESS,  // Master sending address, slave waiting for address, for a read
    I2C_STATE_BUSY_TX,          // Writing data
    I2C_STATE_BUSY_RX,          // Reading data
    I2C_STATE_BUSY_RX_STOP,     // Slave waiting for stop when reading data
    I2C_STATE_COMPLETE,         // Read or write complete
    I2C_STATE_ERROR,            // Read or write failed
    I2C_STATE_WRITE_CALLED,
    I2C_STATE_READ_CALLED,
} i2c_state_t;

#define I2C_EVENT_TRACE

#ifdef I2C_EVENT_TRACE

// Log I2C event interrupts for debugging

#define MAX_FR_ENTRIES 200

typedef struct
{
    i2c_state_t state;
    uint32_t sr1;
    uint32_t sr2;
} i2c_event_t;
i2c_event_t event_log[MAX_FR_ENTRIES];
int next_fr_entry = 0;

#endif

// Store the i2c handle so we can get it in the irq
static i2c_t *i2c_irq_list[2] = {};

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
        // Error
        return UsageFault_IRQn;
    }
}

bool i2c_busy(i2c_t *i2c)
{
    return (   (i2c->state == I2C_STATE_BUSY_START_RX)
            || (i2c->state == I2C_STATE_BUSY_START_TX)
            || (i2c->state == I2C_STATE_BUSY_RX_ADDRESS)
            || (i2c->state == I2C_STATE_BUSY_TX_ADDRESS)
            || (i2c->state == I2C_STATE_BUSY_RX)
            || (i2c->state == I2C_STATE_BUSY_TX)
            || (i2c->state == I2C_STATE_BUSY_RX_STOP));
}

void i2c_clear_read(i2c_t *i2c)
{
    // disable the isr
    I2C_ITConfig(i2c->channel, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, DISABLE);
    // clear the parameters for next read
    i2c->read_buf_len = 0;
    i2c->read_count = 0;
    i2c->read_complete_cb = NULL;
    i2c->cb_param = NULL;
}

void i2c_clear_write(i2c_t *i2c)
{
    // disable the isr
    I2C_ITConfig(i2c->channel, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, DISABLE);
    // clear the buffers for next write
    i2c->write_buf_len = 0;
    i2c->write_count = 0;
    i2c->write_complete_cb = NULL;
    i2c->cb_param = NULL;
}

static void i2c_rx_complete(i2c_t *i2c, int len)
{
    // Copy the transfer parameters before clearing them.
    // They must be cleared, because the callback may queue up another transfer.
    i2c_transfer_complete_cb read_complete_cb = i2c->read_complete_cb;
    void *param = i2c->cb_param;

    i2c_clear_read(i2c);
    if (read_complete_cb != NULL)
    {
        read_complete_cb(i2c, i2c->read_buf, len, param);
    }
}

static void i2c_tx_complete(i2c_t *i2c, int len)
{
    // Disable the interrupts
    I2C_ITConfig(i2c->channel, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, DISABLE);

    // Copy the transfer parameters before clearing them.
    // They must be cleared, because the callback may queue up another transfer.
    i2c_transfer_complete_cb write_complete_cb = i2c->write_complete_cb;
    void *param = i2c->cb_param;

    i2c_clear_write(i2c);
    if (write_complete_cb)
    {
        write_complete_cb(i2c, i2c->write_buf, len, param);
    }
}

// Called from the I2C error interrupt handler
static void i2c_error(i2c_t *i2c)
{
    // Disable the interrupts
    I2C_ITConfig(i2c->channel, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, DISABLE);

    // Copy the transfer parameters before clearing them.
    // They must be cleared, because the callback may queue up another transfer.
    i2c_error_cb error_cb = i2c->error_cb;
    void *param = i2c->cb_param;

    i2c_clear_write(i2c);
    i2c_clear_read(i2c);
    if (error_cb)
    {
        error_cb(i2c, i2c->error_code, param);
    }
}

static void i2c_error_irq_handler(i2c_t *i2c)
{
    if (!i2c)
    {
        return;
    }

    I2C_TypeDef *hi2c = i2c->channel;
    uint32_t sr1 = hi2c->SR1;
    uint32_t cr2 = hi2c->CR2;

    if ((sr1 & I2C_FLAG_BERR) && (cr2 & I2C_IT_ERR))
    {
        // I2C bus error
        i2c->error_code |= I2C_ERROR_BERR;
        // Clear the flag
        hi2c->SR1 = (uint16_t)~I2C_FLAG_BERR;
    }

    if (((sr1 & I2C_FLAG_ARLO) != RESET) && ((cr2 & I2C_IT_ERR) != RESET))
    {
        // I2C Arbitration Loss
        i2c->error_code |= I2C_ERROR_ARLO;
        // Clear the flag
        hi2c->SR1 = (uint16_t)~I2C_FLAG_ARLO;
    }

    if ((sr1 & I2C_FLAG_AF) && (cr2 & I2C_IT_ERR))
    {
        // I2C Acknowledge failure
        i2c->error_code |= I2C_ERROR_AF;

        // Clear the flag
        hi2c->SR1 = (uint16_t)~I2C_FLAG_AF;
    }

    if ((sr1 & I2C_FLAG_OVR) && (cr2 & I2C_IT_ERR))
    {
        // I2C over/under-run
        i2c->error_code |= I2C_ERROR_OVR;
        // Clear the flag
        hi2c->SR1 = (uint16_t)~I2C_FLAG_OVR;
    }

    // Release the bus and notify the caller if we had an error
    if (i2c->error_code != I2C_ERROR_NONE)
    {
        hi2c->CR1 |= I2C_CR1_STOP;
        if (i2c_busy(i2c))
        {
            i2c->state = I2C_STATE_ERROR;
            i2c_error(i2c);
        }
    }
}

// Report an invalid event and cancel the transaction
static void i2c_invalid_event(i2c_t *i2c)
{
    i2c->state = I2C_STATE_ERROR;
    i2c->error_code = I2C_ERROR_INVEVNT;
    i2c_error(i2c);
}

// Address sent or received event
static void i2c_address_event(i2c_t *i2c)
{
    I2C_TypeDef *hi2c = i2c->channel;

    // Slave address sent or received
    if (i2c->state == I2C_STATE_BUSY_RX_ADDRESS)
    {
        // Read
        i2c->state = I2C_STATE_BUSY_RX;
        if (i2c->master && (i2c->read_buf_len == 1))
        {
            // NACK after the first and only byte
            I2C_AcknowledgeConfig(i2c->channel, DISABLE);
        }
        // Enable the interrupt on TXE/RXE
        I2C_ITConfig(i2c->channel, I2C_IT_BUF, ENABLE);
    }
    else if (i2c->state == I2C_STATE_BUSY_TX_ADDRESS)
    {
        // Write
        i2c->state = I2C_STATE_BUSY_TX;
        // Enable the interrupt on TXE/RXE
        I2C_ITConfig(i2c->channel, I2C_IT_BUF, ENABLE);
    }

    // Clear I2C_FLAG_ADDR
    uint32_t temp_reg = hi2c->SR1;
    temp_reg = (hi2c->SR2 << 16);
    (void)temp_reg;
}

// Handle an interrupt when I2C_FLAG_TRA is set
static void i2c_transmit_event(i2c_t *i2c, uint32_t sr1, uint32_t cr2)
{
    I2C_TypeDef *hi2c = i2c->channel;

    if (i2c->state == I2C_STATE_BUSY_TX)
    {
        if (i2c->write_count >= i2c->write_buf_len)
        {
            // All data sent
            if (i2c->master)
            {
                // Generate a stop if we are master
                hi2c->CR1 |= I2C_CR1_STOP;
            }
            i2c->state = I2C_STATE_COMPLETE;
            // Call the callback
            i2c_tx_complete(i2c, i2c->write_count);
        }
        else
        {
            // More data to send
            hi2c->DR = i2c->write_buf[i2c->write_count++];
        }
    }
    else
    {
        // We shouldn't be transmitting
        i2c_invalid_event(i2c);
    }
}

// Byte received event
static void i2c_receive_event(i2c_t *i2c, uint32_t sr1)
{
    I2C_TypeDef *hi2c = i2c->channel;

    if (i2c->state == I2C_STATE_BUSY_RX)
    {
        // Read all available bytes
        while (sr1 & I2C_FLAG_RXNE)
        {
            if (i2c->master && (i2c->read_count >= (i2c->read_buf_len-2)))
            {
                // NACK the last byte
                I2C_AcknowledgeConfig(i2c->channel, DISABLE);
            }
            i2c->read_buf[i2c->read_count++] = hi2c->DR;
            if (i2c->read_count >= i2c->read_buf_len)
            {
                // All data received
                if (i2c->master)
                {
                    // Generate a stop if we are master
                    hi2c->CR1 |= I2C_CR1_STOP;
                    // The receive is complete
                    i2c->state = I2C_STATE_COMPLETE;
                    i2c_rx_complete(i2c, i2c->read_count);
                }
                else
                {
                    // Wait for the stop
                    i2c->state = I2C_STATE_BUSY_RX_STOP;
                    // Disable the TXE/RXE interrupt
                    I2C_ITConfig(i2c->channel, I2C_IT_BUF, DISABLE);
                }
                break;
            }
            sr1 = hi2c->SR1;
        }
    }
    else
    {
        // We shouldn't be receiving, but clear the RXNE
        uint16_t data = hi2c->DR;
        (void)data;
    }
}

// Stop detected
static void i2c_stop_event(i2c_t *i2c)
{
    I2C_TypeDef *hi2c = i2c->channel;

    // Clear the stop bit flag
    //__attribute__((__unused__))
    uint32_t temp_reg = hi2c->SR1;
    (void)temp_reg;
    hi2c->CR1 |= I2C_CR1_PE;

    if (i2c->master)
    {
        // We shouldn't get this event in master mode
        i2c_invalid_event(i2c);
        return;
    }

    if (i2c->state == I2C_STATE_BUSY_TX)
    {
        // Transmitting
        if (i2c->write_count >= i2c->write_buf_len)
        {
            i2c->state = I2C_STATE_COMPLETE;
        }
        else
        {
            i2c->state = I2C_STATE_ERROR;
        }
        // Call the tx complete callback
        i2c_tx_complete(i2c, i2c->write_count);
    }
    else if ((i2c->state == I2C_STATE_BUSY_RX) || (i2c->state == I2C_STATE_BUSY_RX_STOP))
    {
        // Receiving, or waiting for stop at the end of receiving
        if (i2c->read_count >= i2c->read_buf_len)
        {
            i2c->state = I2C_STATE_COMPLETE;
        }
        else
        {
            i2c->state = I2C_STATE_ERROR;
        }
        // Call the rx complete callback
        i2c_rx_complete(i2c, i2c->read_count);
    }
    // else - probably a stop after an ack from the previous transaction.
}

static void i2c_start_event(i2c_t *i2c)
{
    I2C_TypeDef *hi2c = i2c->channel;
    uint32_t sr1;

    // A start condition should only be seen in master mode
    if (i2c->master)
    {
        // Master mode

        // The SB flag is cleared when you read SR1 followed by a write to DR.
        sr1 = hi2c->SR1;
        (void)sr1;
        if (i2c->state == I2C_STATE_BUSY_START_TX)
        {
            // Send the slave write address
            hi2c->DR = i2c->slave_address & 0xFE;
            i2c->state = I2C_STATE_BUSY_TX_ADDRESS;
        }
        else if (i2c->state == I2C_STATE_BUSY_START_RX)
        {
            // Send the slave read address and clear the SB flag
            hi2c->DR = i2c->slave_address | 1;
            i2c->state = I2C_STATE_BUSY_RX_ADDRESS;
        }
        else
        {
            // Invalid state for this event
            i2c_invalid_event(i2c);
        }
    }
    else
    {
        // Cancel the transaction - invalid event for slave mode
        i2c_invalid_event(i2c);
    }
}

static void i2c_irq_handler(i2c_t *i2c)
{
    if (!i2c)
    {
        return;
    }

    I2C_TypeDef *hi2c = i2c->channel;
    // Read SR2 and shift up 16 bits to match bit definitions in stm32f4xx_i2c.h
    uint32_t sr2 = (hi2c->SR2 << 16);
    uint32_t sr1 = hi2c->SR1;
    uint32_t cr2 = hi2c->CR2;

#ifdef I2C_EVENT_TRACE
    event_log[next_fr_entry].state = i2c->state;
    event_log[next_fr_entry].sr1 = sr1;
    event_log[next_fr_entry].sr2 = sr2;
    next_fr_entry++;
    if (next_fr_entry >= MAX_FR_ENTRIES)
    {
        next_fr_entry = 0;
    }
#endif

    // Handle the events that can be handled identically for both slave
    // and master mode
    if ((sr1 & I2C_FLAG_ADDR) && (cr2 & I2C_IT_EVT))
    {
        i2c_address_event(i2c);
    }
    //if (sr2 & I2C_FLAG_TRA)
    {
        // Transmitting
        if (   ((sr1 & I2C_FLAG_TXE) && (cr2 & I2C_IT_BUF))
            || ((sr1 & I2C_FLAG_BTF) && (cr2 & I2C_IT_EVT)))
        {
            // Transmit event
            i2c_transmit_event(i2c, sr1, cr2);
        }
    }
    if ((sr1 & I2C_FLAG_RXNE) && (cr2 & I2C_IT_BUF))
    {
        // Receive event
        i2c_receive_event(i2c, sr1);
    }
    if ((sr1 & I2C_FLAG_SB) && (cr2 & I2C_IT_EVT))
    {
        // Start bit
        i2c_start_event(i2c);
    }
    if ((sr1 & I2C_FLAG_STOPF) && (cr2 & I2C_IT_EVT))
    {
        // STOP received
        i2c_stop_event(i2c);
    }
}

void I2C1_EV_IRQHandler(void)
{
    i2c_irq_handler(i2c_irq_list[0]);
}

void I2C2_EV_IRQHandler(void)
{
    i2c_irq_handler(i2c_irq_list[1]);
}

void I2C1_ER_IRQHandler(void)
{
    i2c_error_irq_handler(i2c_irq_list[0]);
}

void I2C2_ER_IRQHandler(void)
{
    i2c_error_irq_handler(i2c_irq_list[1]);
}

int i2c_read(i2c_t *i2c, uint8_t device_address, void *buf, uint16_t len,
        i2c_transfer_complete_cb cb, i2c_error_cb error_cb, void *param)
{   
    if (len < 1)
    {
        return -1;
    }
    // Make sure we are not interrupted
    sys_enter_critical_section();

    if (i2c_busy(i2c))
    {
        // The I2C driver is busy
        sys_leave_critical_section();
        return -3;
    }

#ifdef I2C_EVENT_TRACE
    I2C_TypeDef *hi2c = i2c->channel;
    uint32_t sr2 = (hi2c->SR2 << 16);
    uint32_t sr1 = hi2c->SR1;

    event_log[next_fr_entry].state = I2C_STATE_READ_CALLED;
    event_log[next_fr_entry].sr1 = sr1;
    event_log[next_fr_entry].sr2 = sr2;
    next_fr_entry++;
    if (next_fr_entry >= MAX_FR_ENTRIES)
    {
        next_fr_entry = 0;
    }
#endif

    // Initialise the read info
    i2c->read_buf_len = len;
    i2c->read_buf = buf;
    i2c->read_count = 0;
    i2c->read_complete_cb = cb;
    i2c->error_cb = error_cb;
    i2c->cb_param = param;
    i2c->slave_address = device_address;
    if (i2c->master)
    {
        i2c->state = I2C_STATE_BUSY_START_RX;
    }
    else
    {
        // In slave mode we don't get a start interrupt
        i2c->state = I2C_STATE_BUSY_RX_ADDRESS;
    }
    // Enable ACK
    I2C_AcknowledgeConfig(i2c->channel, ENABLE);

    // Enable the event and error interrupts
    I2C_ITConfig(i2c->channel, I2C_IT_EVT | I2C_IT_ERR, ENABLE);

    if (i2c->master)
    {
        // Generate a start bit
        I2C_GenerateSTART(i2c->channel, ENABLE);
    }

    sys_leave_critical_section();
    return 0;
}

void i2c_cancel_read(i2c_t *i2c)
{
    sys_enter_critical_section();
    i2c_clear_read(i2c);
    i2c->state = I2C_STATE_IDLE;
    sys_leave_critical_section();

}

int i2c_write(i2c_t *i2c, uint8_t device_address, void *buf, uint16_t len,
        i2c_transfer_complete_cb cb, i2c_error_cb error_cb, void *param)
{
    if (len < 1)
    {
        return -1;
    }
    // Make sure we are not interrupted
    sys_enter_critical_section();

    if (i2c_busy(i2c))
    {
        // The I2C driver is busy
        sys_leave_critical_section();
        return -3;
    }

#ifdef I2C_EVENT_TRACE
    I2C_TypeDef *hi2c = i2c->channel;
    uint32_t sr2 = (hi2c->SR2 << 16);
    uint32_t sr1 = hi2c->SR1;

    event_log[next_fr_entry].state = I2C_STATE_WRITE_CALLED;
    event_log[next_fr_entry].sr1 = sr1;
    event_log[next_fr_entry].sr2 = sr2;
    next_fr_entry++;
    if (next_fr_entry >= MAX_FR_ENTRIES)
    {
        next_fr_entry = 0;
    }
#endif

    // Initialise the write info
    i2c->write_buf_len = len;
    i2c->write_buf = buf;
    i2c->write_count = 0;
    i2c->write_complete_cb = cb;
    i2c->error_cb = error_cb;
    i2c->cb_param = param;
    i2c->slave_address = device_address;
    if (i2c->master)
    {
        i2c->state = I2C_STATE_BUSY_START_TX;
    }
    else
    {
        // In slave mode we don't get a start interrupt
        i2c->state = I2C_STATE_BUSY_TX_ADDRESS;
        // Enable ACK
        I2C_AcknowledgeConfig(i2c->channel, ENABLE);
    }

    // Enable the event and error interrupts
    I2C_ITConfig(i2c->channel, I2C_IT_EVT | I2C_IT_ERR, ENABLE);

    if (i2c->master)
    {
        // Generate a start bit
        I2C_GenerateSTART(i2c->channel, ENABLE);
    }

    sys_leave_critical_section();
    return 0;
}

void i2c_cancel_write(i2c_t *i2c)
{
    sys_enter_critical_section();
    i2c_clear_write(i2c);
    i2c->state = I2C_STATE_IDLE;
    sys_leave_critical_section();
}

i2c_error_code_t i2c_last_error(i2c_t *i2c)
{
    return i2c->error_code;
}

void i2c_init(i2c_t *i2c)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    /* gpio settings */
    gpio_init_pin(i2c->scl);
    gpio_init_pin(i2c->sda);

    /* Reset i2c settings */
    I2C_DeInit(i2c->channel);

    /* Initialise the i2c clock */
    switch((uint32_t)i2c->channel)
    {
    case (uint32_t)I2C1:
        /* Peripheral clock enable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
        /* Reset I2Cx IP */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
        break;
    case (uint32_t)I2C2:
        /* Peripheral clock enable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
        /* Reset I2Cx IP */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, DISABLE);
        break;
    }

    /* i2c isr */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = i2c_irq(i2c);
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure i2c */
    I2C_InitStructure.I2C_Mode = i2c->cfg.I2C_Mode;
    I2C_InitStructure.I2C_OwnAddress1 = i2c->cfg.I2C_OwnAddress1;
    I2C_InitStructure.I2C_Ack = i2c->cfg.I2C_Ack;
    I2C_InitStructure.I2C_AcknowledgedAddress = i2c->cfg.I2C_AcknowledgedAddress;
    I2C_InitStructure.I2C_ClockSpeed = i2c->cfg.I2C_ClockSpeed;
    I2C_InitStructure.I2C_DutyCycle = i2c->cfg.I2C_DutyCycle;

    i2c->state = I2C_STATE_IDLE;
    i2c->error_code = I2C_ERROR_NONE;

    /* Enable i2c after configuration */
    I2C_Init(i2c->channel, &I2C_InitStructure);
    I2C_Cmd(i2c->channel, ENABLE);
}

#ifdef I2C_TEST_READ_CS43L22
// Use this to test I2C master in a Discovery board with the CS43L22 audio chip.
// The following definitions are for the STM32F407G-DISC1

static gpio_pin_t i2c_scl_pb6 =
{
    .port = GPIOB,
    .cfg = {GPIO_Pin_6, GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_OD, GPIO_PuPd_NOPULL},
    .af = GPIO_AF_I2C1
};
static gpio_pin_t i2c_sda_pb9 =
{
    .port = GPIOB,
    .cfg = {GPIO_Pin_9, GPIO_Mode_AF, GPIO_Speed_50MHz, GPIO_OType_OD, GPIO_PuPd_NOPULL},
    .af = GPIO_AF_I2C1
};
static gpio_pin_t audio_reset_pd4 =
{
     .port = GPIOD,
     .cfg = {.GPIO_Pin = GPIO_Pin_4, .GPIO_Mode = GPIO_Mode_OUT,
             .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_OType = GPIO_OType_PP, .GPIO_PuPd = GPIO_PuPd_NOPULL},
 };

static i2c_t i2c_device =
{
    .channel = I2C1,
    .master = true,
    .cfg =
    {
        .I2C_ClockSpeed = 100000,
        .I2C_Mode = I2C_Mode_I2C,
        .I2C_DutyCycle = I2C_DutyCycle_2,
        .I2C_OwnAddress1 = 0x14,
        .I2C_Ack = I2C_Ack_Enable,
        .I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit
    },
    .scl = &i2c_scl_pb6,
    .sda = &i2c_sda_pb9
};

static uint8_t i2c_buf[2];
static bool i2c_done;
static bool i2c_failed;

static void i2c_done_cb(i2c_t *i2c, void *buf, uint16_t len, void *param)
{
    i2c_done = true;
}

static void i2_error_cb(i2c_t *i2c, i2c_error_code_t error_code, void *param)
{
    i2c_done = true;
    i2c_failed = true;
}

// Read the ID register in the CS43L22 and check that the returned value
// is correct.
bool i2c_test_readCS43L22(void)
{
    uint32_t start_time;
    uint32_t now;

    // Take the audio chip out of reset
    gpio_init_pin(&audio_reset_pd4);
    gpio_set_pin(&audio_reset_pd4, 1);

    // Initialise the I2C peripheral
    i2c_init(&i2c_device);

    // Write the register address
    i2c_done = false;
    i2c_failed = false;
    i2c_buf[0] = 0x01;
    if (i2c_write(&i2c_device, 0x94, i2c_buf, 1, i2c_done_cb, i2_error_cb, NULL))
    {
        return false;
    }
    start_time = sys_get_tick();
    while (!i2c_done)
    {
        now = sys_get_tick();
        if ((uint32_t)(now - start_time) > 1000)
        {
            i2c_cancel_write(&i2c_device);
            return false;
        }
    }
    if (i2c_failed)
    {
        return false;
    }

    // Read the register value
    i2c_done = false;
    i2c_failed = false;
    if (i2c_read(&i2c_device, 0x94, i2c_buf, 1, i2c_done_cb, i2_error_cb, NULL) != 0)
    {
        return false;
    }
    start_time = sys_get_tick();
    while (!i2c_done)
    {
        now = sys_get_tick();
        if ((uint32_t)(now - start_time) > 1000)
        {
            i2c_cancel_read(&i2c_device);
            return false;
        }
    }
    if (i2c_failed)
    {
        return false;
    }
    if ((i2c_buf[0] & 0xF8) == 0xE0)
    {
        // Correct CHIPID
        return true;
    }
    return false;
}
#endif
