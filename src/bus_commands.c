#include "ninja.h"
#include "console.h"
#include "bus.h"

#include "bus_commands.h"

need triggers for cmdline start and end
but, that's an ugly hack

static void bus_commands_write(uint8_t c)
{
    console_puts_P(PSTR("WRITE "));
    console_puthex8(c);
    console_newline();
}

static void bus_commands_command(uint8_t *start, size_t len)
{
    console_puts_P(PSTR("COMMAND: "));
    while(0 != len--)
        console_putc(*start++);
    console_newline();
}

const bus_interface_t bus_commands =
{
    .start   = NULL,
    .stop    = NULL,
    .write   = bus_commands_write,
    .read    = NULL,
    .command = bus_commands_command
};

