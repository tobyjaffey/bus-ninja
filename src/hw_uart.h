#ifndef HW_UART_H
#define HW_UART_H 1

#ifndef BAUD
#define BAUD 9600
#endif

void hw_uart_init(void);
void hw_uart_putc(char c);
void hw_uart_tick(void);

#endif

