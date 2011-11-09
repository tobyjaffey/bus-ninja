/*
    usbdev_jtag, by Joby Taffey <jrt@hodgepig.org>
    Based on estick-jtag by Cahya Wirawan <cahya@gmx.at>
    Based on opendous-jtag by Vladimir Fonov and LUFA demo applications by Dean Camera and Denver Gingerich.
    Released under the MIT Licence.
*/

#include <LUFA/Version.h>
#include <LUFA/Drivers/USB/USB.h>

#include "usbdev.h"
#include "usbdev_jtag.h"
#include "jtag.h"
#include "console.h"

/**********************************************************************/

#define IN_EP                       1
#define OUT_EP                      2
#define IN_EP_SIZE                  64
#define OUT_EP_SIZE                 64

#define ESTICK_USB_BUFFER_SIZE          128     //  MUST MATCH openocd/src/jtag/estick.c
#define ESTICK_USB_BUFFER_OFFSET        2
#define ESTICK_IN_BUFFER_SIZE           (ESTICK_USB_BUFFER_SIZE)
#define ESTICK_OUT_BUFFER_SIZE          (ESTICK_USB_BUFFER_SIZE)

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
		
	VendorID:               0x1781,
	ProductID:              0xC0C0,
	ReleaseNumber:          0x0001,
		
	ManufacturerStrIndex:   0x01,
	ProductStrIndex:        0x02,
	SerialNumStrIndex:      0x03,
		
	NumberOfConfigurations: 1
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
extern USB_Descriptor_String_t PROGMEM ManufacturerString;
extern USB_Descriptor_String_t PROGMEM ProductString;

/**********************************************************************/

static uint8_t  usbBuffer[ESTICK_USB_BUFFER_SIZE+ESTICK_USB_BUFFER_OFFSET];
static uint8_t  *dataFromHost = usbBuffer+ESTICK_USB_BUFFER_OFFSET;
static uint8_t  *dataToHost = usbBuffer;
static uint16_t dataFromHostSize = 0;
static uint16_t dataToHostSize = 0;
volatile uint8_t resetJtagTransfers = 0;
static BOOL connected = FALSE;

/**********************************************************************/

void usbdev_jtag_init(void)
{
    jtag_init();

    dataFromHostSize = 0;
    dataToHostSize = 0;
    resetJtagTransfers = 0;
}

void usbdev_jtag_device_connect(BOOL isConnected)
{
    connected = isConnected;
}

void usbdev_jtag_shutdown(void)
{
    // FIXME
}

void usbdev_jtag_tick(void)
{
    if (connected)
	{
        Endpoint_SelectEndpoint(IN_EP);

        if (dataToHostSize && Endpoint_IsReadWriteAllowed())
        {
            if(dataToHostSize)
                Endpoint_Write_Stream_LE(dataToHost,dataToHostSize, NO_STREAM_CALLBACK);
      
            /* Handshake the IN Endpoint - send the data to the host */
            Endpoint_ClearIN();
      
            dataToHostSize=0;
        }

        Endpoint_SelectEndpoint(OUT_EP);

        if (Endpoint_IsReadWriteAllowed())
        {
	        dataFromHostSize = Endpoint_Read_Word_LE();
	        Endpoint_Read_Stream_LE(dataFromHost, dataFromHostSize, NO_STREAM_CALLBACK);
	        Endpoint_ClearOUT();
	        if(dataFromHostSize>0)
            {        
                //first byte is always the command
                dataFromHostSize--;
                dataToHostSize=0;

                switch( dataFromHost[0] &JTAG_CMD_MASK ) 
                {  
                    case JTAG_CMD_TAP_OUTPUT:
                        dataFromHostSize*=4;
                        if( dataFromHost[0] & JTAG_DATA_MASK )
                            dataFromHostSize-= (4- ((dataFromHost[0] & JTAG_DATA_MASK)>>4));
                        if(jtag_get_delay())
                            dataToHostSize= jtag_tap_output_with_delay( &dataFromHost[1] , dataFromHostSize, dataToHost);
                        else
                            dataToHostSize= jtag_tap_output_max_speed( &dataFromHost[1] , dataFromHostSize, dataToHost);
                    break;

                    case JTAG_CMD_TAP_OUTPUT_EMU:
                        dataFromHostSize*=4;
                        if(dataFromHost[0]&JTAG_DATA_MASK)
                            dataFromHostSize-=(4- ((dataFromHost[0]&JTAG_DATA_MASK)>>4));

                        dataToHostSize=jtag_tap_output_emu(&dataFromHost[1], dataFromHostSize, dataToHost);
                    break;

                    case JTAG_CMD_READ_INPUT:
                        dataToHost[0]=jtag_read_input();
                        dataToHostSize=1;
                    break;

                    case JTAG_CMD_SET_SRST:
                        jtag_set_srst(dataFromHost[1]&1);
                        dataToHost[0]=0;//TODO: what to output here?
                        dataToHostSize=1;
                    break;

                    case JTAG_CMD_SET_TRST:
                        jtag_set_trst(dataFromHost[1]&1);
                        dataToHost[0]=0;//TODO: what to output here?
                        dataToHostSize=1;
                    break;

                    case JTAG_CMD_SET_DELAY:
                        jtag_set_delay(dataFromHost[1]*256);
                        dataToHost[0]=0;//TODO: what to output here?
                        dataToHostSize=1;
                    break;

                    case JTAG_CMD_SET_SRST_TRST:
                        jtag_set_trst_srst(dataFromHost[1]&2?1:0,dataFromHost[1]&1);
                        dataToHost[0]=0;//TODO: what to output here?
                        dataToHostSize=1;
                    break;

                    default: //REPORT ERROR?
                    break;
                }
            }
        }
    }
}

uint16_t usbdev_jtag_get_descriptor(const uint16_t wValue, const uint8_t wIndex, void** const DescriptorAddress)
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


void usbdev_jtag_device_configuration_changed(void)
{
	Endpoint_ConfigureEndpoint(IN_EP, EP_TYPE_BULK,
								ENDPOINT_DIR_IN, IN_EP_SIZE,
								ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(OUT_EP, EP_TYPE_BULK,
								ENDPOINT_DIR_OUT, OUT_EP_SIZE,
								ENDPOINT_BANK_SINGLE);

    // pull lines TRST and SRST high
    JTAG_OUT=(1<<JTAG_PIN_TRST)|(1<<JTAG_PIN_SRST);
}

void usbdev_jtag_unhandled_control_request(void)
{
	switch (USB_ControlRequest.bRequest)
	{
		case 0x01:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
                //TODO add reset code here
				/* Write the report data to the control endpoint */
				//Endpoint_Write_Control_Stream_LE(&dataToSend, sizeof(dataToSend));
        
				/* Finalize the transfer, acknowedge the host error or success OUT transfer */
				Endpoint_ClearOUT();
			}
  		break;
	}
}


