#ifndef CONSOLE_H
#define CONSOLE_H 1

#include <avr/pgmspace.h>

typedef void (*putcfunc_t)(uint8_t c);
typedef BOOL (*getcfunc_t)(uint8_t *c);

void console_init(void);
void console_tick(void);

typedef enum
{
    CONSOLE_MODE_LINE,
    CONSOLE_MODE_KEY
} console_mode_t;

void console_set_mode(console_mode_t mode);

void console_set_echo(BOOL newecho);
void console_set_silent(BOOL newsilent);

void console_set_putc(putcfunc_t f);

void console_putc(uint8_t c);
BOOL console_getc(uint8_t *c);
BOOL console_key_poll(uint8_t *c);

void console_newline(void);
void console_puts(uint8_t *str);
void console_puts_P(PGM_P str);
void console_puthex8(uint8_t h);
void console_puthex16(uint16_t h);
void console_put0x8(uint8_t h);
void console_putdec(uint32_t i);
void console_putbin(uint8_t b);
void console_putsmem(const uint8_t *a, const uint8_t *b);

void console_rx_callback(uint8_t c);
BOOL console_rx_ready_callback(void);

#endif

