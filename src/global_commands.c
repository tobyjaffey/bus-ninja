#include "ninja.h"
#include "global_commands.h"
#include "parse.h"
#include "console.h"
#include "cpu.h"
#include "led.h"

#include "bus.h"

#ifdef CONFIG_BUS_SPI
#include "bus_spi.h"
#endif

#ifdef CONFIG_BUS_I2C
#include "bus_i2c.h"
#endif

#ifdef CONFIG_USBDEV
#include "usbdev.h"
#endif

#ifdef CONFIG_COMMAND_BITBANG
#include "command_bitbang.h"
#endif

#ifdef CONFIG_COMMAND_MENUTEST
#include "menu.h"
#endif

/****************************************************************************************/
// Declare function names and prototypes

DECLARE_COMMAND(version)
#ifdef CONFIG_COMMAND_MENUTEST
DECLARE_COMMAND(menutest)
#endif
#ifdef CONFIG_COMMAND_DUMPKEYS
DECLARE_COMMAND(dumpkeys)
#endif
#ifdef CONFIG_COMMAND_BITBANG
DECLARE_COMMAND(bitbang)
#endif
#ifdef CONFIG_COMMAND_TEST
DECLARE_COMMAND(test)
#endif
DECLARE_COMMAND(help)
DECLARE_COMMAND(led)
#ifdef COMMAND_ECHO
DECLARE_COMMAND(echo)
#endif
#ifdef COMMAND_SILENT
DECLARE_COMMAND(silent)
#endif
#ifdef COMMAND_LOG
DECLARE_COMMAND(log)
#endif
DECLARE_COMMAND(reset)
#ifdef CONFIG_BUS_SPI
DECLARE_COMMAND(spi)
#endif
#ifdef CONFIG_BUS_I2C
DECLARE_COMMAND(i2c)
#endif
#ifdef CONFIG_USBDEV_JTAG
DECLARE_COMMAND(jtag)
#endif
#ifdef CONFIG_USBDEV_AVRISP
DECLARE_COMMAND(avrisp)
#endif

/****************************************************************************************/
// Create table of names and function pointers

const global_command_t global_command_table[] PROGMEM =
{
    ADD_COMMAND(version)
#ifdef CONFIG_COMMAND_MENUTEST
    ADD_COMMAND(menutest)
#endif
#ifdef CONFIG_COMMAND_DUMPKEYS
    ADD_COMMAND(dumpkeys)
#endif
#ifdef CONFIG_COMMAND_BITBANG
    ADD_COMMAND(bitbang)
#endif
#ifdef CONFIG_COMMAND_TEST
    ADD_COMMAND(test)
#endif
    ADD_COMMAND(help)
#ifdef COMMAND_LOG
    ADD_COMMAND(log)
#endif
    ADD_COMMAND(led)
#ifdef COMMAND_ECHO
    ADD_COMMAND(echo)
#endif
#ifdef COMMAND_SILENT
    ADD_COMMAND(silent)
#endif
    ADD_COMMAND(reset)
#ifdef CONFIG_BUS_SPI
    ADD_COMMAND(spi)
#endif
#ifdef CONFIG_BUS_I2C
    ADD_COMMAND(i2c)
#endif
#ifdef CONFIG_USBDEV_AVRISP
    ADD_COMMAND(avrisp)
#endif
    {NULL, NULL},
};


/****************************************************************************************/
// Define command handlers

#define TOK_NUM(p_tokend, p_num)    tok_num(&tok_start, p_tokend, line_end, p_num)

#ifdef CONFIG_COMMAND_TEST
// read all available tokens, converting to numbers as possible
// TODO, basis for custom parsers in bus_modules
DEFINE_COMMAND(test)
{
    const uint8_t *tok_end;
    uint32_t num;

    while(tok_start != line_end)
    {
        if (TOK_NUM(&tok_end, &num))
        {
            console_puts_P(PSTR("number: "));
            console_putdec(num);
            console_newline();
        }
        else
        {
            console_puts_P(PSTR("string: "));
            console_putsmem(tok_start, tok_end);
            console_newline();
            tok_start = tok_end;
        }
    }

    return TRUE;
}
#endif


DEFINE_COMMAND(version)
{
    console_puts_P(PSTR(VERSION_STR));
    console_newline();
    return TRUE;
}

#ifdef CONFIG_COMMAND_MENUTEST
DEFINE_COMMAND(menutest)
{
    mymenu();
    return TRUE;
}
#endif

#ifdef CONFIG_COMMAND_BITBANG
DEFINE_COMMAND(bitbang)
{
    command_bitbang();
    return TRUE;
}
#endif

#ifdef CONFIG_COMMAND_DUMPKEYS
DEFINE_COMMAND(dumpkeys)
{
    uint8_t c;

    console_set_mode(CONSOLE_MODE_KEY);
    console_puts_P(PSTR("Space to quit"));
    console_newline();

    do
    {
        while(!console_key_poll(&c))
            driver_tick();

        console_puthex8(c);
        console_newline();
    }
    while(c != ' ');

    console_set_mode(CONSOLE_MODE_LINE);
    
    return TRUE;
}
#endif

DEFINE_COMMAND(led)
{
    const uint8_t *tokp;
    uint32_t num;

    if (!TOK_NUM(&tokp, &num))
        return FALSE;

    if (num >= LED_SEQ_MAX)
        return FALSE;

    led_set_seq(num);
    return TRUE;
}

#ifdef COMMAND_SILENT
DEFINE_COMMAND(silent)
{
    const uint8_t *tokp;
    uint32_t num;

    if (!TOK_NUM(&tokp, &num))
        return FALSE;

    console_set_silent(num == 1);
    return TRUE;
}
#endif

#ifdef COMMAND_ECHO
DEFINE_COMMAND(echo)
{
    const uint8_t *tokp;
    uint32_t num;

    if (!TOK_NUM(&tokp, &num))
        return FALSE;

    console_set_echo(num == 1);
    return TRUE;
}
#endif

#ifdef COMMAND_LOG
DEFINE_COMMAND(log)
{
    const uint8_t *tokp;
    uint32_t num;

    if (!TOK_NUM(&tokp, &num))
        return FALSE;

    bus_enable_log(num == 1);
    return TRUE;
}
#endif

DEFINE_COMMAND(help)
{
    const global_command_t *cmd_ptr = global_command_table;
    global_command_t cmd;

    memcpy_P(&cmd, cmd_ptr, sizeof(global_command_t));
    while(NULL != cmd.name)
    {
        console_puts_P(cmd.name);
        console_newline();
        memcpy_P(&cmd, ++cmd_ptr, sizeof(global_command_t));
    }
    return TRUE;
}

DEFINE_COMMAND(reset)
{
    cpu_reset();
    return TRUE;
}

#ifdef CONFIG_BUS_SPI
DEFINE_COMMAND(spi)
{
    bus_init(&bus_spi);
    return TRUE;
}
#endif

#ifdef CONFIG_BUS_I2C
DEFINE_COMMAND(i2c)
{
    bus_init(&bus_i2c);
    return TRUE;
}
#endif

#ifdef CONFIG_USBDEV_JTAG
DEFINE_COMMAND(jtag)
{
    usbdev_init(USBMODE_JTAG);
    return TRUE;
}
#endif

#ifdef CONFIG_USBDEV_AVRISP
DEFINE_COMMAND(avrisp)
{
    usbdev_init(USBMODE_AVRISP);
    return TRUE;
}
#endif


