#include "ninja.h"
#include "console.h"
#include "bus.h"
#include "hw_spi.h"

#include "bus_spi.h"

static void bus_spi_open(void)
{
    hw_spi_init();
}

static void bus_spi_close(void)
{
    hw_spi_shutdown();
}

static void bus_spi_start(void)
{
    hw_spi_cs_assert();

    bus_log_puts_P(PSTR("CS ENABLED"));
    bus_log_newline();
}

static void bus_spi_stop(void)
{
    hw_spi_cs_deassert();

    bus_log_puts_P(PSTR("CS DISABLED"));
    bus_log_newline();
}

static void bus_spi_write(uint8_t c)
{
    hw_spi_write8(c);
    bus_log_puts_P(PSTR("WRITE: 0x"));
    bus_log_puthex8(c);
    bus_log_newline();
}

static void bus_spi_read(void)
{
    uint8_t c;
    c = hw_spi_write8(0xFF);
    bus_log_puts_P(PSTR("READ: 0x"));
    bus_out(c);
    bus_log_newline();
}

static BOOL bus_spi_command(const uint8_t *start, size_t len, uint32_t repeat)
{
    return FALSE;
}

const bus_interface_t bus_spi =
{
    .open       = bus_spi_open,
    .close      = bus_spi_close,
    .start      = bus_spi_start,
    .stop       = bus_spi_stop,
    .write      = bus_spi_write,
    .read       = bus_spi_read,
    .command    = bus_spi_command
};

