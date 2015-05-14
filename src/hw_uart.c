#include <avr/io.h>
#include <avr/pgmspace.h>

#include "hw_uart.h"
#include "usbdev_cdc.h"
#include "console.h"
#include "watchdog.h"

#include <util/setbaud.h>       /* BAUD may be defined in hw_uart.h */

void hw_uart_init(void)
{
#if __AVR_ATmega168__ || __AVR_ATmega328P__
#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~_BV(U2X0);
#endif
    UBRR0 = UBRR_VALUE;
    UCSR0B = _BV(TXEN0) | _BV(RXEN0);
#elif __AVR_ATmega8__
#if USE_2X
    UCSRA |=  _BV(U2X);
#else
    UCSRA &= ~_BV(U2X);
#endif
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;
    UCSRB = _BV(TXEN) | _BV(RXEN);
#elif __AVR_AT90USB162__
#if USE_2X
#error U2X not supported on AT90USB162
#endif
    UBRR1 = UBRR_VALUE;
    UCSR1B = _BV(TXEN1) | _BV(RXEN1);
#else
#error Unsupported device, FIXME
#endif
}

void hw_uart_tick(void)
{
#ifdef CONFIG_HW_UART_CONSOLE
#if __AVR_ATmega168__ || __AVR_ATmega328P__
    if ((UCSR0A&(1<<RXC0)) != 0)
    {
        uint8_t c = UDR0;
        console_rx_callback(c);
    }
#elif __AVR_ATmega8__
    if ((UCSRA&(1<<RXC)) != 0) {
        uint8_t c = UDR;
        console_rx_callback(c);
    }
#elif __AVR_AT90USB162__
    if ((UCSR1A&(1<<RXC1)) != 0)
    {
        uint8_t c = UDR1;
        console_rx_callback(c);
    }
#else
#error Unsupported device, FIXME
#endif
#endif
}

void hw_uart_putc(char c)
{
#if __AVR_ATmega168__ || __AVR_ATmega328P__
    while (bit_is_clear(UCSR0A, UDRE0))
        watchdog_reset();
    UDR0 = c;
#elif __AVR_ATmega8__
    while (bit_is_clear(UCSRA, UDRE))
        watchdog_reset();
    UDR = c;
#elif __AVR_AT90USB162__
    while (bit_is_clear(UCSR1A, UDRE1))
        watchdog_reset();
    UDR1 = c;
#else
#error Unsupported device, FIXME
#endif
}



