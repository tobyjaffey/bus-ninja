#include <LUFA/Version.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Drivers/USB/Class/CDC.h>
#include <LUFA/Drivers/USB/Class/MassStorage.h>
#include <LUFA/Drivers/USB/USB.h>

#include "usbdev.h"

#ifdef CONFIG_USBDEV_CDC
#include "usbdev_cdc.h"
#endif
#ifdef CONFIG_USBDEV_JTAG
#include "usbdev_jtag.h"
#endif
#ifdef CONFIG_USBDEV_AVRISP
#include "usbdev_avrisp.h"
#endif

/**********************************************************************/

static uint8_t usbMode = USBMODE_NULL;
static BOOL connected = FALSE;

/**********************************************************************/

USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
	.Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},
		
	.USBSpecification       = VERSION_BCD(01.10),
	.Class                  = 0x00,
	.SubClass               = 0x00,
	.Protocol               = 0x00,
				
	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,
		
	.VendorID               = 0x03EB,
	.ProductID              = 0x2044,
	.ReleaseNumber          = 0x0000,
		
	.ManufacturerStrIndex   = 0x01,
	.ProductStrIndex        = 0x02,
	.SerialNumStrIndex      = USE_INTERNAL_SERIAL,
		
	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

USB_Descriptor_String_t PROGMEM LanguageString =
{
	.Header                 = {.Size = USB_STRING_LEN(1), .Type = DTYPE_String},
		
	.UnicodeString          = {LANGUAGE_ID_ENG}
};

USB_Descriptor_String_t PROGMEM ManufacturerString =
{
	.Header                 = {.Size = USB_STRING_LEN(11), .Type = DTYPE_String},
		
	.UnicodeString          = L"StoneyLabs"
};

USB_Descriptor_String_t PROGMEM ProductString =
{
	.Header                 = {.Size = USB_STRING_LEN(13), .Type = DTYPE_String},
		
	.UnicodeString          = L"BusNinja"
};

/**********************************************************************/

void usbdev_init(uint8_t mode)
{
    switch(usbMode)
    {
#ifdef CONFIG_USBDEV_CDC
        case USBMODE_CDC:
            usbdev_cdc_shutdown();
            break;
#endif
#ifdef CONFIG_USBDEV_JTAG
        case USBMODE_JTAG:
            usbdev_jtag_shutdown();
            break;
#endif
#ifdef CONFIG_USBDEV_AVRISP
        case USBMODE_AVRISP:
            usbdev_avrisp_shutdown();
            break;
#endif
    }

    if (usbMode != USBMODE_NULL)
        USB_ShutDown();

    usbMode = mode;

    switch(usbMode)
    {
#ifdef CONFIG_USBDEV_CDC
        case USBMODE_CDC:
            usbdev_cdc_init();
            break;
#endif
#ifdef CONFIG_USBDEV_JTAG
        case USBMODE_JTAG:
            usbdev_jtag_init();
            break;
#endif
#ifdef CONFIG_USBDEV_AVRISP
        case USBMODE_AVRISP:
            usbdev_avrisp_init();
            break;
#endif
    }

	USB_Init();
}

void usbdev_tick(void)
{
    if (connected)
    {
        switch(usbMode)
        {
#ifdef CONFIG_USBDEV_CDC
            case USBMODE_CDC:
                usbdev_cdc_tick();
                break;
#endif
#ifdef CONFIG_USBDEV_JTAG
            case USBMODE_JTAG:
                usbdev_jtag_tick();
                break;
#endif
#ifdef CONFIG_USBDEV_AVRISP
            case USBMODE_AVRISP:
                usbdev_avrisp_tick();
                break;
#endif
        }
    }

    USB_USBTask();
}

void EVENT_USB_Device_Connect(void)
{
    connected = TRUE;
    switch(usbMode)
    {
#ifdef CONFIG_USBDEV_CDC
        case USBMODE_CDC:
            usbdev_cdc_device_connect(TRUE);
            break;
#endif
#ifdef CONFIG_USBDEV_JTAG
        case USBMODE_JTAG:
            usbdev_jtag_device_connect(TRUE);
            break;
#endif
#ifdef CONFIG_USBDEV_AVRISP
        case USBMODE_AVRISP:
            usbdev_avrisp_device_connect(TRUE);
            break;
#endif
    }
}

void EVENT_USB_Device_Disconnect(void)
{
    connected = FALSE;
    switch(usbMode)
    {
#ifdef CONFIG_USBDEV_CDC
        case USBMODE_CDC:
            usbdev_cdc_device_connect(FALSE);
            break;
#endif
#ifdef CONFIG_USBDEV_JTAG
        case USBMODE_JTAG:
            usbdev_jtag_device_connect(FALSE);
            break;
#endif
#ifdef CONFIG_USBDEV_AVRISP
        case USBMODE_AVRISP:
            usbdev_avrisp_device_connect(FALSE);
            break;
#endif
    }
}

void EVENT_USB_Device_ConfigurationChanged(void)
{
    switch(usbMode)
    {
#ifdef CONFIG_USBDEV_CDC
        case USBMODE_CDC:
            usbdev_cdc_device_configuration_changed();
            break;
#endif
#ifdef CONFIG_USBDEV_JTAG
        case USBMODE_JTAG:
            usbdev_jtag_device_configuration_changed();
            break;
#endif
#ifdef CONFIG_USBDEV_AVRISP
        case USBMODE_AVRISP:
            usbdev_avrisp_device_configuration_changed();
            break;
#endif
    }
}

void EVENT_USB_Device_UnhandledControlRequest(void)
{
    switch(usbMode)
    {
#ifdef CONFIG_USBDEV_CDC
        case USBMODE_CDC:
            usbdev_cdc_unhandled_control_request();
            break;
#endif
#ifdef CONFIG_USBDEV_JTAG
        case USBMODE_JTAG:
            usbdev_jtag_unhandled_control_request();
            break;
#endif
#ifdef CONFIG_USBDEV_AVRISP
        case USBMODE_AVRISP:
            usbdev_avrisp_unhandled_control_request();
            break;
#endif
    }
}

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint8_t wIndex, void** const DescriptorAddress)
{
    switch(usbMode)
    {
#ifdef CONFIG_USBDEV_CDC
        case USBMODE_CDC:
            return usbdev_cdc_get_descriptor(wValue, wIndex, DescriptorAddress);
            break;
#endif
#ifdef CONFIG_USBDEV_JTAG
        case USBMODE_JTAG:
            return usbdev_jtag_get_descriptor(wValue, wIndex, DescriptorAddress);
            break;
#endif
#ifdef CONFIG_USBDEV_AVRISP
        case USBMODE_AVRISP:
            return usbdev_avrisp_get_descriptor(wValue, wIndex, DescriptorAddress);
            break;
#endif
        default:
            return 0;   // can't happen
    }
}
