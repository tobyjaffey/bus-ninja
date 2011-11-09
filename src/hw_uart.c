#include <avr/io.h>
#include <avr/pgmspace.h>

#include "hw_uart.h"
#include "usbdev_cdc.h"
#include "console.h"
#include "watchdog.h"

#define BAUD 9600

void hw_uart_init(void)
{
#if __AVR_ATmega168__
    UBRR0 = (F_CPU / (16UL * BAUD)) - 1;
    UCSR0B = _BV(TXEN0) | _BV(RXEN0);
#elif __AVR_AT90USB162__
    UBRR1 = (F_CPU / (16UL * BAUD)) - 1;
    UCSR1B = _BV(TXEN1) | _BV(RXEN1);
#else
#error Unsupported device, FIXME
#endif
}

void hw_uart_tick(void)
{
#ifdef CONFIG_HW_UART_CONSOLE
#if __AVR_ATmega168__
    if ((UCSR0A&(1<<RXC0)) != 0)
    {
        uint8_t c = UDR0;
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
#if __AVR_ATmega168__
    while (bit_is_clear(UCSR0A, UDRE0))
        watchdog_reset();
    UDR0 = c;
#elif __AVR_AT90USB162__
    while (bit_is_clear(UCSR1A, UDRE1))
        watchdog_reset();
    UDR1 = c;
#else
#error Unsupported device, FIXME
#endif
}



