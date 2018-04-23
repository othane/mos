/**
 * @file usb.c
 *
 * @brief USB custom HID driver for the stm32f4
 *
 * @author Charles Oram
 *
 * @date Mar 2018
 *
 */

#include "hal.h"
#include "usbd_req.h"
#include "usbd_ioreq.h"
#include "usb_bsp.h"
#include <string.h>


extern uint32_t USBD_OTG_ISR_Handler(USB_OTG_CORE_HANDLE *pdev);

// Functions to get the USB descriptors
static uint8_t *get_device_descriptor(uint8_t speed, uint16_t *length);
static uint8_t *get_language_id_descriptor(uint8_t speed, uint16_t *length);
static uint8_t *get_manufacturer_string_descriptor(uint8_t speed, uint16_t *length);
static uint8_t *get_product_string_descriptor(uint8_t speed, uint16_t *length);
static uint8_t *get_serial_number_string_descriptor(uint8_t speed, uint16_t *length);
static uint8_t *get_config_string_descriptor(uint8_t speed, uint16_t *length);
static uint8_t *get_user_string_descriptor(uint8_t speed, uint8_t index, uint16_t *length);
static uint8_t *get_interface_string_descriptor(uint8_t speed, uint16_t *length);
static void usb_hid_device_configured(void);
// Empty callbacks
static void dummy_callback(void);
static void dummy_callback2(uint8_t param1);

// Table of functions to retrieve USB descriptors
static USBD_DEVICE get_descriptors_table =
{
	  get_device_descriptor,
	  get_language_id_descriptor,
	  get_manufacturer_string_descriptor,
	  get_product_string_descriptor,
	  get_serial_number_string_descriptor,
	  get_config_string_descriptor,
	  get_interface_string_descriptor
};

// Offset into the configuration descriptor for finding the custom HID descriptor.
#define HID_DESCRIPTOR_OFFSET       18
#define HID_DESCRIPTOR_LEN          9

// Custom HID interface callbacks
static uint8_t usb_device_hid_open(void *pdev, uint8_t cfgidx);
static uint8_t usb_device_hid_close(void *pdev, uint8_t cfgidx);
static uint8_t usb_device_hid_setup(void *pdev, USB_SETUP_REQ *req);
static uint8_t *usb_device_hid_get_config_desc(uint8_t speed, uint16_t *length);
static uint8_t usb_device_hid_data_in(void *pdev, uint8_t epnum);
static uint8_t usb_device_hid_data_out(void *pdev, uint8_t epnum);
static uint8_t usb_device_hid_ep0_rx_ready(void *pdev);

static USBD_Class_cb_TypeDef usb_device_hid_callbacks =
{
  usb_device_hid_open,
  usb_device_hid_close,
  usb_device_hid_setup,
  NULL, /*EP0_TxSent*/
  usb_device_hid_ep0_rx_ready,
  usb_device_hid_data_in,
  usb_device_hid_data_out,
  NULL, /*SOF */
  NULL,
  NULL,
  usb_device_hid_get_config_desc,
  get_user_string_descriptor
};

// User event callbacks structure.
// Initialise all callbacks to a default callback that does nothing
// because the STM32 library does not check whether the callbacks
// have been initialised.
static USBD_Usr_cb_TypeDef usb_usr_event_callbacks =
{
    .Init = dummy_callback,
    .DeviceConnected = dummy_callback,
    .DeviceDisconnected = dummy_callback,
    .DeviceConfigured = usb_hid_device_configured,
    .DeviceReset = dummy_callback2,
    .DeviceSuspended = dummy_callback,
    .DeviceResumed = dummy_callback
};

// Pointer to the user supplied configuration
static usb_config_t *usb_configuration;
// Handle to the USB device
static USB_OTG_CORE_HANDLE usb_dev_handle;
// Buffer for receiving report packets
static uint8_t usb_receive_buf[USB_OTG_FS_MAX_PACKET_SIZE];
// Flag to indicate when there is a received packet available in the buffer
static int usb_receive_buffer_full = false;
// Buffer for sending report packets
static uint8_t usb_send_buf[USB_OTG_FS_MAX_PACKET_SIZE];
static uint8_t hid_report_id = 0;
static __IO uint32_t report_available = 0;
static volatile int report_sent;
static uint32_t hid_alt_set = 0;
static uint32_t hid_protocol = 0;
static uint32_t hid_idle_state = 0;
// Temporary string descriptor buffer
static uint8_t temp_string_descriptor[USB_MAX_STR_DESC_SIZ];

// Implementation

static void usb_hid_start_read(usb_dev_handle_t dev)
{
    USB_OTG_CORE_HANDLE *pdev = (USB_OTG_CORE_HANDLE *)dev;
    DCD_EP_PrepareRx(pdev, usb_configuration->out_endpoint_address, usb_receive_buf, usb_configuration->out_max_packet_size);
}

// Called when the device has connected and enumerated
static void usb_hid_device_configured(void)
{
    // The USB HID connection is up, so start reading from it
    usb_hid_start_read((void *)&usb_dev_handle);

    // Call the user supplied callback, if configured
    if (usb_configuration->configured_cb)
    {
        usb_configuration->configured_cb();
    }

}

static uint8_t *get_device_descriptor(uint8_t speed, uint16_t *length)
{
    *length = sizeof(usb_device_descriptor_t);
    return (uint8_t*)usb_configuration->device_descriptor;
}

static uint8_t *get_language_id_descriptor(uint8_t speed, uint16_t *length)
{
	*length =  sizeof(lang_id_descriptor_t);
	return (uint8_t*)usb_configuration->lang_id_descriptor;
}

static uint8_t *get_manufacturer_string_descriptor(uint8_t speed, uint16_t *length)
{
	USBD_GetString((uint8_t *)usb_configuration->manufacturer_string, temp_string_descriptor, length);
	return temp_string_descriptor;
}

static uint8_t *get_product_string_descriptor(uint8_t speed, uint16_t *length)
{
	USBD_GetString((uint8_t *)usb_configuration->product_string, temp_string_descriptor, length);
	return temp_string_descriptor;
}

static uint8_t *get_serial_number_string_descriptor(uint8_t speed, uint16_t *length)
{
	USBD_GetString((uint8_t *)usb_configuration->serial_number_string, temp_string_descriptor, length);
	return temp_string_descriptor;
}

static uint8_t *get_config_string_descriptor(uint8_t speed, uint16_t *length)
{
	USBD_GetString((uint8_t *)usb_configuration->configuration_string, temp_string_descriptor, length);
	return temp_string_descriptor;
}

// Get a user string descriptor by searching the user provided array of string
// and index pairs.
static uint8_t *get_user_string_descriptor(uint8_t speed, uint8_t index, uint16_t *length)
{
    if (usb_configuration->num_user_strings)
    {
        int i;
        for (i = 0; i < usb_configuration->num_user_strings; i++)
        {
            if (index == usb_configuration->user_strings[i].index)
            {
                USBD_GetString((uint8_t *)usb_configuration->user_strings[i].string, temp_string_descriptor, length);
                return temp_string_descriptor;
            }
        }
    }

    // Unknown string index
    USBD_GetString((uint8_t *)"?", temp_string_descriptor, length);
    return temp_string_descriptor;
}

static uint8_t *get_interface_string_descriptor(uint8_t speed, uint16_t *length)
{
    USBD_GetString((uint8_t *)usb_configuration->interface_string, temp_string_descriptor, length);
    return temp_string_descriptor;
}

// Dummy callbacks that are used for event callbacks that have not been specified by the application
static void dummy_callback(void)
{
}
static void dummy_callback2(uint8_t param1)
{
}

// USB Custom HID device callbacks

static uint8_t usb_device_hid_open(void *pdev, uint8_t cfgidx)
{
    // OK to send a new report
    report_sent = 1;
    /* Open the IN endpoint */
    DCD_EP_Open(pdev, usb_configuration->in_endpoint_address, usb_configuration->in_max_packet_size, USB_OTG_EP_INT);
    /* Open OUT endpoint */
    DCD_EP_Open(pdev, usb_configuration->out_endpoint_address, usb_configuration->out_max_packet_size, USB_OTG_EP_INT);
    return USBD_OK;
}

static uint8_t usb_device_hid_close(void *pdev, uint8_t cfgidx)
{
    /* Close HID endpoints */
    DCD_EP_Close(pdev, usb_configuration->in_endpoint_address);
    DCD_EP_Close(pdev, usb_configuration->out_endpoint_address);
    return USBD_OK;
}

static uint8_t usb_device_hid_setup(void *pdev, USB_SETUP_REQ *req)
{
    uint8_t report_length = 0;
    uint16_t len = 0;
    uint8_t *pbuf = NULL;

    switch (req->bmRequest & USB_REQ_TYPE_MASK)
    {
    case USB_REQ_TYPE_CLASS :
        switch (req->bRequest)
        {
        case CUSTOM_HID_REQ_SET_PROTOCOL:
            hid_protocol = (uint8_t)(req->wValue);
            break;
        case CUSTOM_HID_REQ_GET_PROTOCOL:
            USBD_CtlSendData(pdev, (uint8_t *)&hid_protocol, 1);
            break;
        case CUSTOM_HID_REQ_SET_IDLE:
            hid_idle_state = (uint8_t)(req->wValue >> 8);
            break;
        case CUSTOM_HID_REQ_GET_IDLE:
            USBD_CtlSendData(pdev, (uint8_t *)&hid_idle_state, 1);
            break;
        case CUSTOM_HID_REQ_SET_REPORT:
            report_available = 1;
            hid_report_id = (uint8_t)(req->wValue);
            report_length = (uint8_t)(req->wLength);
            USBD_CtlPrepareRx(pdev, usb_receive_buf, report_length);
            break;
        default:
            USBD_CtlError(pdev, req);
            return USBD_FAIL;
        }
        break;

    case USB_REQ_TYPE_STANDARD:
        switch (req->bRequest)
        {
        case USB_REQ_GET_DESCRIPTOR:
            if ((req->wValue >> 8) == CUSTOM_HID_REPORT_DESC_TYPE)
            {
                len = MIN(usb_configuration->hid_report_descriptor_len, req->wLength);
                pbuf = (uint8_t*)usb_configuration->hid_report_descriptor;
            }
            else if ((req->wValue >> 8) == CUSTOM_HID_DESCRIPTOR_TYPE)
            {
                pbuf = (uint8_t*)usb_configuration->config_descriptor + HID_DESCRIPTOR_OFFSET;
                len = MIN(HID_DESCRIPTOR_LEN, req->wLength);
            }
            USBD_CtlSendData(pdev, pbuf, len);
            break;
        case USB_REQ_GET_INTERFACE :
            USBD_CtlSendData(pdev, (uint8_t *)&hid_alt_set, 1);
            break;
        case USB_REQ_SET_INTERFACE :
            hid_alt_set = (uint8_t)(req->wValue);
            break;
        }
    }
    return USBD_OK;
}

static uint8_t *usb_device_hid_get_config_desc(uint8_t speed, uint16_t *length)
{
    *length = usb_configuration->config_descriptor_len;
    return usb_configuration->config_descriptor;
}

// Called when a packet has been sent
static uint8_t usb_device_hid_data_in(void *pdev, uint8_t epnum)
{
    /* Ensure that the FIFO is empty before a new transfer, this condition could
    be caused by  a new transfer before the end of the previous transfer */
    DCD_EP_Flush(pdev, usb_configuration->in_endpoint_address);

    if (epnum == 1)
    {
        // Report has been sent
        report_sent = 1;
    }

    return USBD_OK;
}

// Called when a packet has been received
static uint8_t usb_device_hid_data_out(void *pdev, uint8_t epnum)
{
    if (epnum == 1)
    {
        // Mark the receive buffer as holding a packet
        usb_receive_buffer_full = 1;
    }
    return USBD_OK;
}

static uint8_t usb_device_hid_ep0_rx_ready(void *pdev)
{
    if (report_available == 1)
    {
        report_available = 0;
    }
    return USBD_OK;
}

// USB BSP functions.
// These are called by the USB OTG device driver to initialise the hardware and setup/run interrupts

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);

  /* Configure SOF ID DM DP Pins */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12;

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_OTG1_FS) ;
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_OTG1_FS) ;
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_OTG1_FS) ;

  /* Configure  VBUS Pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure ID pin */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_OTG1_FS) ;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE) ;

  /* enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
}

// Enable the OTG FS device interrupt
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

// Delay for a given number of microseconds
void USB_OTG_BSP_uDelay(const uint32_t usec)
{
    uint32_t count = 0;
    const uint32_t utime = (120 * usec / 7);
    do
    {
        if (++count > utime)
        {
            return ;
        }
    }
    while (1);
}

// Delay for a given number of milliseconds
void USB_OTG_BSP_mDelay (const uint32_t msec)
{
    sys_spin(msec);
}

void OTG_FS_IRQHandler(void)
{
    USBD_OTG_ISR_Handler(&usb_dev_handle);
}

// Interface functions

usb_dev_handle_t usb_init(usb_config_t *usb_config)
{
    // Get a pointer to the USB configuration
    usb_configuration = usb_config;

    if (usb_config->init_event_cb)
    {
        usb_usr_event_callbacks.Init = usb_config->init_event_cb;
    }
    if (usb_config->connected_cb)
    {
        usb_usr_event_callbacks.DeviceConnected = usb_config->connected_cb;
    }
    if (usb_config->disconnected_cb)
    {
        usb_usr_event_callbacks.DeviceDisconnected = usb_config->disconnected_cb;
    }
    // DeviceConfigured is handled differently, because we need to catch that callback
    // before the user does.
    if (usb_config->configured_cb)
    {
        usb_usr_event_callbacks.DeviceConfigured = usb_config->configured_cb;
    }
    if (usb_config->reset_cb)
    {
        usb_usr_event_callbacks.DeviceReset = usb_config->reset_cb;
    }
    if (usb_config->suspended_cb)
    {
        usb_usr_event_callbacks.DeviceSuspended = usb_config->suspended_cb;
    }
    if (usb_config->resumed_cb)
    {
        usb_usr_event_callbacks.DeviceResumed = usb_config->resumed_cb;
    }

    USBD_Init(&usb_dev_handle, USB_OTG_FS_CORE_ID, &get_descriptors_table, &usb_device_hid_callbacks, &usb_usr_event_callbacks);
    return (usb_dev_handle_t)&usb_dev_handle;
}

// Non-blocking read of a USB HID report
int usb_hid_read(usb_dev_handle_t dev, uint8_t report_id, uint8_t *buf, int len)
{
    if (usb_receive_buffer_full && (usb_receive_buf[0] == report_id))
    {
        // Data has been received, for this report ID. Copy it into the user's buffer
        if (len > usb_configuration->out_report_len)
        {
            len = usb_configuration->out_report_len;
        }
        // Skip the report ID when copying
        memcpy(buf, &usb_receive_buf[1], len);
        // Queue up another read
        usb_receive_buffer_full = false;
        usb_hid_start_read(dev);
        return len;
    }
    else
    {
        // No data available, for this report ID
        return 0;
    }
}

// Non-blocking write of a USB HID report
usb_return_code_t usb_hid_send_report(usb_dev_handle_t dev, uint8_t report_id, uint8_t *report, int len)
{
    USB_OTG_CORE_HANDLE *pdev = (USB_OTG_CORE_HANDLE *)dev;
    if (pdev->dev.device_status != USB_OTG_CONFIGURED)
    {
        // The USB interface is not yet connected
        return USB_RC_FAIL;
    }
    if (!report_sent)
    {
        // Still busy sending the last report
        return USB_RC_BUSY;
    }
    if (len > usb_configuration->in_report_len)
    {
        return USB_RC_TOO_BIG;
    }

    report_sent = 0;

    // Add the report ID
    usb_send_buf[0] = report_id;
    // Copy the data into the send buffer
    memcpy(&usb_send_buf[1], report, len);
    if (len < usb_configuration->in_report_len)
    {
        // Zero unused data in the report
        memset(&usb_send_buf[len+1], 0, usb_configuration->in_report_len-len);
    }
    // Send the report ID and report data.
    // Reports are always a fixed size, even though the user data may not fill the report.
    DCD_EP_Tx(pdev, usb_configuration->in_endpoint_address, usb_send_buf, usb_configuration->in_report_len+1);
    return USB_RC_OK;
}

// End of file
