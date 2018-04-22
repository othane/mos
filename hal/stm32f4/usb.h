/**
 * @file usb.h
 *
 * @brief USB custom HID driver for the stm32f4
 *
 * @author Charles Oram
 *
 * @date Mar 2018
 *
 */

#ifndef _USB_H_
#define _USB_H_

#include "usbd_def.h"

typedef enum
{
    USB_RC_OK,
    USB_RC_FAIL,
    USB_RC_BUSY,
    USB_RC_TOO_BIG
} usb_return_code_t;

typedef void *usb_dev_handle_t;

// USB Standard Device Descriptor
typedef struct
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t bcdUSB;
  uint8_t bDeviceClass;
  uint8_t bDeviceSubClass;
  uint8_t bDeviceProtocol;
  uint8_t bMaxPacketSize0;
  uint16_t idVendor;
  uint16_t idProduct;
  uint16_t bcdDevice;
  uint8_t iManufacturer;
  uint8_t iProduct;
  uint8_t iSerialNumber;
  uint8_t bNumConfigurations;
} usb_device_descriptor_t __attribute__((packed));

// USB Language ID Descriptor
typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wLANGID;
} lang_id_descriptor_t __attribute__((packed));

// Note: The other descriptor types are too complicated or are variable
// length, so cannot be described using a structure.

// USB descriptor types
#define USB_DEVICE_DESCRIPTOR_TYPE             1
#define USB_CONFIGURATION_DESCRIPTOR_TYPE      2
#define USB_STRING_DESCRIPTOR_TYPE             3
#define USB_INTERFACE_DESCRIPTOR_TYPE          4
#define USB_ENDPOINT_DESCRIPTOR_TYPE           5
#define USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE   6
#define USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE 7
#define USB_INTERFACE_POWER_DESCRIPTOR_TYPE    8

// USB device classes
#define USB_DEVICE_CLASS_RESERVED              0x00
#define USB_DEVICE_CLASS_AUDIO                 0x01
#define USB_DEVICE_CLASS_COMMUNICATIONS        0x02
#define USB_DEVICE_CLASS_HUMAN_INTERFACE       0x03
#define USB_DEVICE_CLASS_MONITOR               0x04
#define USB_DEVICE_CLASS_PHYSICAL_INTERFACE    0x05
#define USB_DEVICE_CLASS_POWER                 0x06
#define USB_DEVICE_CLASS_PRINTER               0x07
#define USB_DEVICE_CLASS_STORAGE               0x08
#define USB_DEVICE_CLASS_HUB                   0x09
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC       0xFF

// bmAttributes in configuration descriptor
#define USB_BUS_POWERED                 0x80
#define USB_SELF_POWERED                0xC0
#define USB_REMOTE_WAKEUP               0x20

// Endpoint addresses
#define USB_ENDPOINT_OUT(addr)  (addr)
#define USB_ENDPOINT_IN(addr)  ((addr) | 0x80)

// bmAttributes in endpoint descriptor
#define USB_ENDPOINT_TYPE_MASK                 0x03
#define USB_ENDPOINT_TYPE_CONTROL              0x00
#define USB_ENDPOINT_TYPE_ISOCHRONOUS          0x01
#define USB_ENDPOINT_TYPE_BULK                 0x02
#define USB_ENDPOINT_TYPE_INTERRUPT            0x03
#define USB_ENDPOINT_SYNC_MASK                 0x0C
#define USB_ENDPOINT_SYNC_NO_SYNCHRONIZATION   0x00
#define USB_ENDPOINT_SYNC_ASYNCHRONOUS         0x04
#define USB_ENDPOINT_SYNC_ADAPTIVE             0x08
#define USB_ENDPOINT_SYNC_SYNCHRONOUS          0x0C
#define USB_ENDPOINT_USAGE_MASK                0x30
#define USB_ENDPOINT_USAGE_DATA                0x00
#define USB_ENDPOINT_USAGE_FEEDBACK            0x10
#define USB_ENDPOINT_USAGE_IMPLICIT_FEEDBACK   0x20
#define USB_ENDPOINT_USAGE_RESERVED            0x30

// HID descriptor types
#define CUSTOM_HID_DESCRIPTOR_TYPE           0x21
#define CUSTOM_HID_REPORT_DESC_TYPE          0x22
#define CUSTOM_HID_PHYSICAL_DESC_TYPE        0x23

// HID request types
#define CUSTOM_HID_REQ_SET_PROTOCOL          0x0B
#define CUSTOM_HID_REQ_GET_PROTOCOL          0x03
#define CUSTOM_HID_REQ_SET_IDLE              0x0A
#define CUSTOM_HID_REQ_GET_IDLE              0x02
#define CUSTOM_HID_REQ_SET_REPORT            0x09
#define CUSTOM_HID_REQ_GET_REPORT            0x01

#define USBD_LANGID_US                       0x409

#define USB_FS_MAX_PACKET_SIZE               64

// User defined strings are passed to the USB driver as an array of
// index and string pairs.
typedef struct
{
    int index;
    char *string;
} user_strings_t;

typedef struct
{
    usb_device_descriptor_t *device_descriptor;
    uint8_t *config_descriptor;
    int config_descriptor_len;
    lang_id_descriptor_t *lang_id_descriptor;
    uint8_t *hid_report_descriptor;
    int hid_report_descriptor_len;
    char *manufacturer_string;
    char *product_string;
    char *configuration_string;
    char *interface_string;
    char *serial_number_string;
    user_strings_t *user_strings;   // Array of user strings
    int num_user_strings;           // Number of entries in user_strings
    uint8_t in_endpoint_address;
    uint8_t out_endpoint_address;
    uint8_t in_max_packet_size;
    uint8_t out_max_packet_size;
    uint8_t in_report_len;  // Length of the data, excluding the report ID
    uint8_t out_report_len; // Length of the data, excluding the report ID
    uint8_t in_report_id;
    uint8_t out_report_id;

    // Device event callbacks
    void (*init_event_cb)(void);
    void (*reset_cb)(uint8_t speed);
    void (*configured_cb)(void);
    void (*suspended_cb)(void);
    void (*resumed_cb)(void);
    void (*connected_cb)(void);
    void (*disconnected_cb)(void);
} usb_config_t;

usb_dev_handle_t usb_init(usb_config_t *usb_config);
usb_return_code_t usb_hid_send_report(usb_dev_handle_t dev, uint8_t report_id, uint8_t *report, int len);
int usb_hid_read(usb_dev_handle_t dev, uint8_t report_id, uint8_t *buf, int len);

#endif
