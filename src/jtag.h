#ifndef JTAG_H
#define JTAG_H 1

#include "ninja.h"

//jtag i/o pins
#define JTAG_OUT PORTB
#define JTAG_IN  PINB
#define JTAG_DIR DDRB

//output pins
#define JTAG_PIN_TDI  0
#define JTAG_PIN_TMS  1
#define JTAG_PIN_TRST 2
#define JTAG_PIN_SRST 3
#define JTAG_PIN_TCK  4
//input pins
#define JTAG_PIN_TDO  5
#define JTAG_PIN_EMU  6
#define JTAG_PIN_RTCK 7

//JTAG usb commands
//TODO: ADD commands to deal with RTCK ?
//TODO: maybe add commands to query some firmware info...
#define JTAG_CMD_TAP_OUTPUT     0x0
#define JTAG_CMD_SET_TRST       0x1
#define JTAG_CMD_SET_SRST       0x2
#define JTAG_CMD_READ_INPUT     0x3
#define JTAG_CMD_TAP_OUTPUT_EMU 0x4
#define JTAG_CMD_SET_DELAY      0x5
#define JTAG_CMD_SET_SRST_TRST  0x6

//JTAG usb command mask
#define JTAG_CMD_MASK       0x0f
#define JTAG_DATA_MASK      0xf0

//JTAG pins masks
#define JTAG_OUTPUT_MASK ((1<<JTAG_PIN_TDI)|(1<<JTAG_PIN_TMS)|(1<<JTAG_PIN_TRST)|(1<<JTAG_PIN_SRST)|(1<<JTAG_PIN_TCK))
#define JTAG_INPUT_MASK  ((1<<JTAG_PIN_TDO)|(1<<JTAG_PIN_EMU)|(1<<JTAG_PIN_RTCK))
#define JTAG_SIGNAL_MASK ((1<<JTAG_PIN_TDI)|(1<<JTAG_PIN_TMS))

#define JTAG_CLK_LO  ~(1<<JTAG_PIN_TCK)
#define JTAG_CLK_HI   (1<<JTAG_PIN_TCK)

//additional delay to make clk hi and lo approximately the same length, not sure if this is really needed
#define JTAG_DELAY2 20


//! initialize JTAG interface
void jtag_init(void);

//! send taps through JTAG interface and recieve responce from TDO pin only
//! \parameter out_buffer - buffer of taps for output, data is packed TDI and TMS values a stored together 
//! \parameter out_length - total number of pairs to send (maximum length is 4*255 samples)
//! \parameter in_buffer  - buffer which will hold recieved data data will be packed 
//! \return    number of bytes used in the in_buffer 
uint8_t jtag_tap_output_max_speed(const uint8_t *out_buffer, uint16_t out_length, uint8_t *in_buffer);

//! send taps through JTAG interface and recieve responce from TDO pin only
//! \parameter out_buffer - buffer of taps for output, data is packed TDI and TMS values a stored together 
//! \parameter out_length - total number of pairs to send (maximum length is 4*255 samples)
//! \parameter in_buffer  - buffer which will hold recieved data data will be packed 
//! \return    number of bytes used in the in_buffer 
uint8_t jtag_tap_output_with_delay(const uint8_t *out_buffer, uint16_t out_length, uint8_t *in_buffer);

//! send taps through JTAG interface and recieve responce from TDO and EMU pins 
//! \parameter out_buffer - buffer of taps for output, data is packed TDI and TMS values a stored together 
//! \parameter out_length - total number of pairs to send (maximum length is 4*255 samples)
//! \parameter in_buffer  - buffer which will hold recieved data data will be packed 
//! \return    number of bytes used in the in_buffer (equal to the input (length+3)/4
uint8_t jtag_tap_output_emu(const uint8_t *out_buffer,uint16_t out_length,uint8_t *in_buffer);


//! return current status of TDO & EMU pins
//! \return packed result TDO - bit 0 , EMU bit 1
uint8_t jtag_read_input(void);

//! set pin TRST 
void jtag_set_trst(uint8_t trst);

//! set pin SRST 
void jtag_set_srst(uint8_t srst);

//! set both srst and trst simultaneously
void jtag_set_trst_srst(uint8_t trst,uint8_t srst);

uint16_t jtag_get_delay(void);

void jtag_set_delay(uint16_t delay);

#endif

