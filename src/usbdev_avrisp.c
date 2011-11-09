/*
 * AVR In-System-Programming over USB
 * Intended to be a clone of USBasp, http://www.fischl.de/usbasp/
 * USBasp is supported in AVRDude 5.2+
 *
 * Status - successfully fakes some transactions
 * The other side of this protocol is here - http://svn.savannah.nongnu.org/viewvc/trunk/avrdude/usbasp.c?root=avrdude&view=markup
 *
 */

#include <LUFA/Version.h>
#include <LUFA/Drivers/USB/USB.h>

#include "usbdev.h"
#include "usbdev_avrisp.h"
#include "console.h"

/**********************************************************************/

#define IN_EP                       1
#define OUT_EP                      2
#define IN_EP_SIZE                  64
#define OUT_EP_SIZE                 64

/**********************************************************************/

typedef struct
{
    USB_Descriptor_Configuration_Header_t Config;
    USB_Descriptor_Interface_t            Interface;
    USB_Descriptor_Endpoint_t             DataINEndpoint;
    USB_Descriptor_Endpoint_t             DataOUTEndpoint;
} USB_Descriptor_Configuration_t;

/**********************************************************************/

static USB_Descriptor_Device_t PROGMEM DeviceDescriptor  =
{
	Header:                 {Size: sizeof(USB_Descriptor_Device_t), Type: DTYPE_Device},
		
	USBSpecification:       VERSION_BCD(01.10),
	Class:                  0x00,
	SubClass:               0x00,	
	Protocol:               0x00,
				
	Endpoint0Size:          8,
		
	VendorID:               0x16C0,
	ProductID:              0x05DC,
	ReleaseNumber:          0x0001,
		
	ManufacturerStrIndex:   0x01,
	ProductStrIndex:        0x02,
	SerialNumStrIndex:      0x03,
		
	NumberOfConfigurations: 1
};

/**********************************************************************/

USB_Descriptor_String_t PROGMEM avrisp_ProductString =
{
	.Header                 = {.Size = USB_STRING_LEN(13), .Type = DTYPE_String},
		
	.UnicodeString          = L"USBasp"
};

/**********************************************************************/

USB_Descriptor_String_t PROGMEM avrisp_ManufacturerString =
{
	.Header                 = {.Size = USB_STRING_LEN(13), .Type = DTYPE_String},
		
	.UnicodeString          = L"www.fischl.de"
};

/**********************************************************************/

static USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor  =
{
	Config:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Configuration_Header_t), Type: DTYPE_Configuration},

			TotalConfigurationSize: sizeof(USB_Descriptor_Configuration_t),
			TotalInterfaces:        1,
				
			ConfigurationNumber:    1,
			ConfigurationStrIndex:  NO_DESCRIPTOR,
				
			ConfigAttributes:       (USB_CONFIG_ATTR_BUSPOWERED | USB_CONFIG_ATTR_SELFPOWERED),
			
			MaxPowerConsumption:    USB_CONFIG_POWER_MA(100)
		},
		
	Interface:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Interface_t), Type: DTYPE_Interface},

			InterfaceNumber:        0x00,
			AlternateSetting:       0x00,
			
			TotalEndpoints:         2,
				
			Class:                  0xFF,
			SubClass:               0x00,
			Protocol:               0x00,
				
			InterfaceStrIndex:      NO_DESCRIPTOR
		},

	DataINEndpoint:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Endpoint_t), Type: DTYPE_Endpoint},

			EndpointAddress:        (ENDPOINT_DESCRIPTOR_DIR_IN | IN_EP),
			Attributes:             EP_TYPE_BULK,
			EndpointSize:           IN_EP_SIZE,
			PollingIntervalMS:      0x00
		},

	DataOUTEndpoint:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Endpoint_t), Type: DTYPE_Endpoint},

			EndpointAddress:        (ENDPOINT_DESCRIPTOR_DIR_OUT | OUT_EP),
			Attributes:             EP_TYPE_BULK,
			EndpointSize:           OUT_EP_SIZE,
			PollingIntervalMS:      0x00
		}
};

/**********************************************************************/

extern USB_Descriptor_String_t PROGMEM LanguageString;

/**********************************************************************/

static BOOL connected = FALSE;

/**********************************************************************/

void usbdev_avrisp_init(void)
{
    console_puts_P(PSTR("init"));
    console_newline();
}

void usbdev_avrisp_device_connect(BOOL isConnected)
{
    console_puts_P(PSTR("connect"));
    console_newline();

    connected = isConnected;
}

void usbdev_avrisp_shutdown(void)
{
    console_puts_P(PSTR("shutdown"));
    console_newline();
}


void usbdev_avrisp_tick(void)
{
#if 0
    if (connected)
	{
        Endpoint_SelectEndpoint(IN_EP);

        if (replyBufLen && Endpoint_IsReadWriteAllowed())
        {
            console_puts_P(PSTR("Write "));
            console_putdec(replyBufLen);
            console_newline();

            Endpoint_Write_Stream_LE( replyBuf, replyBufLen, NO_STREAM_CALLBACK);
      
            /* Handshake the IN Endpoint - send the data to the host */
            Endpoint_ClearIN();
            replyBufLen = 0;
        }

        Endpoint_SelectEndpoint(OUT_EP);

        if (Endpoint_IsReadWriteAllowed())
        {
            uint8_t tmp[16];
            uint8_t tmplen;

	        tmplen = Endpoint_Read_Word_LE();
	        Endpoint_Read_Stream_LE(tmp, tmplen, NO_STREAM_CALLBACK);
	        Endpoint_ClearOUT();

            console_puts_P(PSTR("r/w "));
            console_putdec(tmplen);
            console_newline();

#if 0
	        if(dataFromHostSize>0)
            {        
                //first byte is always the command
                dataFromHostSize--;
                dataToHostSize=0;

                switch( dataFromHost[0] &avrisp_CMD_MASK ) 
                {  
                    default: //REPORT ERROR?
                    break;
                }
            }
#endif
        }
    }
#endif
}

uint16_t usbdev_avrisp_get_descriptor(const uint16_t wValue, const uint8_t wIndex, void** const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	void*          Address = NULL;
	uint16_t       Size    = NO_DESCRIPTOR;

    console_puts_P(PSTR("getdesc"));
    console_newline();

	switch (DescriptorType)
	{
		case DTYPE_Device:
			Address = (void*)&DeviceDescriptor;
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration: 
            Address = (void*)&ConfigurationDescriptor;
            Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String: 
			switch (DescriptorNumber)
			{
				case 0x00: 
					Address = (void*)&LanguageString;
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case 0x01: 
					Address = (void*)&avrisp_ManufacturerString;
					Size    = pgm_read_byte(&avrisp_ManufacturerString.Header.Size);
					break;
				case 0x02: 
					Address = (void*)&avrisp_ProductString;
					Size    = pgm_read_byte(&avrisp_ProductString.Header.Size);
					break;
			}
			break;
	}
	
	*DescriptorAddress = Address;		
	return Size;
}


void usbdev_avrisp_device_configuration_changed(void)
{
    console_puts_P(PSTR("configchange"));
    console_newline();

	Endpoint_ConfigureEndpoint(IN_EP, EP_TYPE_BULK,
								ENDPOINT_DIR_IN, IN_EP_SIZE,
								ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(OUT_EP, EP_TYPE_BULK,
								ENDPOINT_DIR_OUT, OUT_EP_SIZE,
								ENDPOINT_BANK_SINGLE);
}

#define USBASP_FUNC_CONNECT     1
#define USBASP_FUNC_DISCONNECT  2
#define USBASP_FUNC_TRANSMIT    3
#define USBASP_FUNC_READFLASH   4
#define USBASP_FUNC_ENABLEPROG  5
#define USBASP_FUNC_WRITEFLASH  6
#define USBASP_FUNC_READEEPROM  7
#define USBASP_FUNC_WRITEEEPROM 8
#define USBASP_FUNC_SETLONGADDRESS 9
#define USBASP_FUNC_SETISPSCK 10

void usbdev_avrisp_unhandled_control_request(void)
{
    static uint8_t replyBuf[8];

    console_puts_P(PSTR("bRequest = "));
    console_puthex8(USB_ControlRequest.bRequest);
    console_newline();
    console_puts_P(PSTR("wValue = "));
    console_puthex8(USB_ControlRequest.wValue);
    console_newline();
    console_puts_P(PSTR("wIndex = "));
    console_puthex8(USB_ControlRequest.wIndex);
    console_newline();

	switch (USB_ControlRequest.bRequest)
	{
        case USBASP_FUNC_CONNECT:
            replyBuf[0] = 0;
            Endpoint_ClearSETUP();
            Endpoint_Write_Control_Stream_LE(replyBuf, 1);
            Endpoint_ClearOUT();
            break;

        case USBASP_FUNC_DISCONNECT:
            replyBuf[0] = 0;
            Endpoint_ClearSETUP();
            Endpoint_Write_Control_Stream_LE(replyBuf, 1);
            Endpoint_ClearOUT();
            break;

        case USBASP_FUNC_SETISPSCK:
            replyBuf[0] = 0;
            Endpoint_ClearSETUP();
            Endpoint_Write_Control_Stream_LE(replyBuf, 1);
            Endpoint_ClearOUT();
            break;

        case USBASP_FUNC_ENABLEPROG:
            replyBuf[0] = 0;
            Endpoint_ClearSETUP();
            Endpoint_Write_Control_Stream_LE(replyBuf, 1);
            Endpoint_ClearOUT();
            break;

        case USBASP_FUNC_TRANSMIT:
            replyBuf[0] = 0x14;
            replyBuf[1] = 0x03;
            replyBuf[2] = 0x94;
            replyBuf[3] = 0x1e;
            Endpoint_ClearSETUP();
            Endpoint_Write_Control_Stream_LE(replyBuf, 4);
            Endpoint_ClearOUT();
            break;
	}
}


