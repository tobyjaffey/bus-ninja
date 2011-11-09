#ifndef BUS_H
#define BUS_H 1

#include "ninja.h"

typedef struct
{
    void (*open)(void);
    void (*close)(void);
    void (*start)(void);
    void (*stop)(void);
    void (*write)(uint8_t c);
    void (*read)(void);
    BOOL (*command)(const uint8_t *start, size_t len, uint32_t repeat);
} bus_interface_t;

/*****************************************************************************/

// Formatting mode for bus_out()
typedef enum
{
    OUTMODE_0x8,    // 2 byte hex, with 0x prefix
    OUTMODE_ASCII,  // ascii char
    OUTMODE_BIN,    // binary
    OUTMODE_DEC,    // decimal
    OUTMODE_HEX8,   // 2 byte hex
    OUTMODE_SILENT  // silent
} outmode_t;

/*****************************************************************************/

extern const bus_interface_t *current_bus;

/*****************************************************************************/

void bus_init(const bus_interface_t *bus);

/*****************************************************************************/

void bus_set_outmode(outmode_t mode);
void bus_out(uint8_t c);

/*****************************************************************************/

void bus_enable_log(BOOL enable);
void bus_log_putc(uint8_t c);
void bus_log_puthex8(uint8_t c);
void bus_log_put0x8(uint8_t c);
void bus_log_puts(uint8_t *str);
void bus_log_puts_P(PGM_P str);
void bus_log_newline(void);
void bus_log_putdec(uint32_t i);

#endif

