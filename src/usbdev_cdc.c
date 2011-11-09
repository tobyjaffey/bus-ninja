#include <LUFA/Version.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Drivers/USB/Class/CDC.h>
#include <avr/wdt.h>

#include "usbdev.h"
#include "watchdog.h"
#include "usbdev_cdc.h"
#include "console.h"

/**********************************************************************/

#define CDC_FUNCTIONAL_DESCRIPTOR(DataSize)        \
    struct                                        \
    {                                             \
      USB_Descriptor_Header_t Header;          \
      uint8_t                 SubType;         \
      uint8_t                 Data[DataSize];  \
    }

typedef struct
{
    USB_Descriptor_Configuration_Header_t    Config;
    USB_Descriptor_Interface_t               CDC_CCI_Interface;
    CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC_Functional_IntHeader;
    CDC_FUNCTIONAL_DESCRIPTOR(1)             CDC_Functional_AbstractControlManagement;
    CDC_FUNCTIONAL_DESCRIPTOR(2)             CDC_Functional_Union;
    USB_Descriptor_Endpoint_t                CDC_ManagementEndpoint;
    USB_Descriptor_Interface_t               CDC_DCI_Interface;
    USB_Descriptor_Endpoint_t                CDC_DataOutEndpoint;
    USB_Descriptor_Endpoint_t                CDC_DataInEndpoint;
} USB_Descriptor_Configuration_CDC_t;


#define CDC_NOTIFICATION_EPNUM         1
#define CDC_TX_EPNUM                   2	
#define CDC_RX_EPNUM                   3	
#define CDC_NOTIFICATION_EPSIZE        8
#define CDC_TXRX_EPSIZE                CDC_BUFSIZE

enum
{
    CONTROL_INTERFACE_NUM=0,
    DATA_INTERFACE_NUM=1
};

static USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
{
    .Config =
    {
        .ControlInterfaceNumber         = CONTROL_INTERFACE_NUM,
        .DataINEndpointNumber           = CDC_TX_EPNUM,
        .DataINEndpointSize             = CDC_TXRX_EPSIZE,
        .DataINEndpointDoubleBank       = false,
        .DataOUTEndpointNumber          = CDC_RX_EPNUM,
        .DataOUTEndpointSize            = CDC_TXRX_EPSIZE,
        .DataOUTEndpointDoubleBank      = false,
        .NotificationEndpointNumber     = CDC_NOTIFICATION_EPNUM,
        .NotificationEndpointSize       = CDC_NOTIFICATION_EPSIZE,
        .NotificationEndpointDoubleBank = false,
    },
};

USB_Descriptor_Configuration_CDC_t PROGMEM ConfigurationDescriptor_CDC =
{
	.Config = 
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

			.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_CDC_t),
			.TotalInterfaces        = 2,
				
			.ConfigurationNumber    = 1,
			.ConfigurationStrIndex  = NO_DESCRIPTOR,
				
			.ConfigAttributes       = (USB_CONFIG_ATTR_BUSPOWERED | USB_CONFIG_ATTR_SELFPOWERED),
			
			.MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
		},

	.CDC_CCI_Interface = 
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

			.InterfaceNumber        = CONTROL_INTERFACE_NUM,
			.AlternateSetting       = 0,
			
			.TotalEndpoints         = 1,
				
			.Class                  = 0x02,
			.SubClass               = 0x02,
			.Protocol               = 0x01,
				
			.InterfaceStrIndex      = NO_DESCRIPTOR
		},

	.CDC_Functional_IntHeader = 
		{
			.Header                 = {.Size = sizeof(CDC_FUNCTIONAL_DESCRIPTOR(2)), .Type = 0x24},
			.SubType                = 0x00,
			
			.Data                   = {0x01, 0x10}
		},

	.CDC_Functional_AbstractControlManagement = 
		{
			.Header                 = {.Size = sizeof(CDC_FUNCTIONAL_DESCRIPTOR(1)), .Type = 0x24},
			.SubType                = 0x02,
			
			.Data                   = {0x06}
		},
		
	.CDC_Functional_Union = 
		{
			.Header                 = {.Size = sizeof(CDC_FUNCTIONAL_DESCRIPTOR(2)), .Type = 0x24},
			.SubType                = 0x06,
			
			.Data                   = {0x00, 0x01}
		},

	.CDC_ManagementEndpoint = 
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
										 
			.EndpointAddress        = (ENDPOINT_DESCRIPTOR_DIR_IN | CDC_NOTIFICATION_EPNUM),
			.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = CDC_NOTIFICATION_EPSIZE,
			.PollingIntervalMS      = 0xFF
		},

	.CDC_DCI_Interface = 
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

			.InterfaceNumber        = DATA_INTERFACE_NUM,
			.AlternateSetting       = 0,
			
			.TotalEndpoints         = 2,
				
			.Class                  = 0x0A,
			.SubClass               = 0x00,
			.Protocol               = 0x00,
				
			.InterfaceStrIndex      = NO_DESCRIPTOR
		},

	.CDC_DataOutEndpoint = 
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
										 
			.EndpointAddress        = (ENDPOINT_DESCRIPTOR_DIR_OUT | CDC_RX_EPNUM),
			.Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = CDC_TXRX_EPSIZE,
			.PollingIntervalMS      = 0x00
		},
		
	.CDC_DataInEndpoint = 
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
										 
			.EndpointAddress        = (ENDPOINT_DESCRIPTOR_DIR_IN | CDC_TX_EPNUM),
			.Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = CDC_TXRX_EPSIZE,
			.PollingIntervalMS      = 0x00
		},
};

/**********************************************************************/

extern USB_Descriptor_Device_t PROGMEM DeviceDescriptor;
extern USB_Descriptor_String_t PROGMEM LanguageString;
extern USB_Descriptor_String_t PROGMEM ManufacturerString;
extern USB_Descriptor_String_t PROGMEM ProductString;

/**********************************************************************/

static usbdev_cdc_rx_cb_func_t rx_cb = NULL;
static usbdev_cdc_rx_ready_cb_func_t rx_ready_cb = NULL;
static BOOL connected = FALSE;

/**********************************************************************/

void usbdev_cdc_init(void)
{
}

void usbdev_cdc_device_connect(BOOL isConnected)
{
    connected = isConnected;
#ifdef CONFIG_USBDEV_CDC_CONSOLE
    if (isConnected)
    {
        console_set_putc(usbdev_cdc_putc);
        usbdev_cdc_set_rx_cb(console_rx_callback);
        usbdev_cdc_set_rx_ready_cb(console_rx_ready_callback);
    }
    else
    {
        console_set_putc(NULL);
        usbdev_cdc_set_rx_cb(NULL);
        usbdev_cdc_set_rx_ready_cb(NULL);
    }
#endif
}

void usbdev_cdc_set_rx_cb(usbdev_cdc_rx_cb_func_t f)
{  
    rx_cb = f;
}

void usbdev_cdc_set_rx_ready_cb(usbdev_cdc_rx_ready_cb_func_t f)
{  
    rx_ready_cb = f;
}

void usbdev_cdc_shutdown(void)
{
#ifdef CONFIG_USBDEV_CDC_CONSOLE
    console_set_putc(NULL);
#endif
}

void usbdev_cdc_tick(void)
{
    uint8_t count = 0;

    if (!connected)
        return;

    // empty CDC rx buffer into rx_cb
    while (count++ < CDC_BUFSIZE && CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface) > 0)
    {
        uint8_t c;

        if (NULL != rx_ready_cb && (*rx_ready_cb)())
        {
            c = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
            if (NULL != rx_cb)
                (*rx_cb)(c);
            else
                break;
        }
        else
            break;
    }

    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
}

void usbdev_cdc_putc(uint8_t c)
{
    // force byte out of CDC immediately
    watchdog_reset();

    CDC_Device_SendByte(&VirtualSerial_CDC_Interface, c);   // write
    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);       // flush
    USB_USBTask();  // is this necessary?
}


uint16_t usbdev_cdc_get_descriptor(const uint16_t wValue, const uint8_t wIndex, void** const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	void*          Address = NULL;
	uint16_t       Size    = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
		case DTYPE_Device:
			Address = (void*)&DeviceDescriptor;
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration: 
            Address = (void*)&ConfigurationDescriptor_CDC;
            Size    = sizeof(USB_Descriptor_Configuration_CDC_t);
			break;
		case DTYPE_String: 
			switch (DescriptorNumber)
			{
				case 0x00: 
					Address = (void*)&LanguageString;
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case 0x01: 
					Address = (void*)&ManufacturerString;
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case 0x02: 
					Address = (void*)&ProductString;
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
			}
			break;
	}
	
	*DescriptorAddress = Address;		
	return Size;
}


void usbdev_cdc_device_configuration_changed(void)
{
    CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
}

void usbdev_cdc_unhandled_control_request(void)
{
    CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}


