#include "ninja.h"
#include "bus.h"
#include "console.h"

const bus_interface_t *current_bus = NULL;
static uint8_t outmode = OUTMODE_0x8;   // output formatting for bus data
static BOOL log_enabled = TRUE;

/****************************************************************/

void bus_init(const bus_interface_t *bus)
{
    if (NULL != current_bus && NULL != current_bus->close)
        current_bus->close();

    current_bus = bus;

    if (NULL != bus && NULL != bus->open)
        bus->open();
}

/****************************************************************/

void bus_enable_log(BOOL enable)
{
    log_enabled = enable;
}

/****************************************************************/

void bus_set_outmode(outmode_t mode)
{
    outmode = mode;
}

/****************************************************************/

void bus_out(uint8_t c)
{
    switch(outmode)
    {
        case OUTMODE_ASCII:
            console_putc(c);
            break;
        case OUTMODE_BIN:
            console_putbin(c);
            break;
        case OUTMODE_DEC:
            console_putdec(c);
            break;
        case OUTMODE_HEX8:
            console_puthex8(c);
            break;
        case OUTMODE_0x8:
            console_put0x8(c);
            break;
    }
    // put a separator between bytes
    if (!log_enabled)
        console_putc(' ');
}

/****************************************************************/

void bus_log_putc(uint8_t c)
{
    if (log_enabled)
        console_putc(c);
}

void bus_log_puthex8(uint8_t c)
{
    if (log_enabled)
        console_puthex8(c);
}

void bus_log_put0x8(uint8_t c)
{
    if (log_enabled)
        console_put0x8(c);
}

void bus_log_puts(uint8_t *str)
{
    if (log_enabled)
        console_puts(str);
}

void bus_log_puts_P(PGM_P str)
{
    if (log_enabled)
        console_puts_P(str);
}

void bus_log_newline(void)
{
    if (log_enabled)
        console_newline();
}

void bus_log_putdec(uint32_t i)
{
    if (log_enabled)
        console_putdec(i);
}

