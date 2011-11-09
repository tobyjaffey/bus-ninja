#include "ninja.h"
#include "console.h"
#include "command_bitbang.h"

void command_bitbang(void)
{
    uint8_t c;
    uint8_t exitCount = 0;

    console_puts_P(PSTR("Entering binary bitbang mode, +++ to exit"));
    console_set_mode(CONSOLE_MODE_KEY);

    do
    {
        DDRB = 0x00;

        while(!console_key_poll(&c))
        {
            driver_tick();

            if (c == '+')
                exitCount++;
            else
                exitCount = 0;
console_putc(PINB);
//            console_puthex8(PINB);
//            console_newline();
        }
    }
    while(exitCount < 3);

    console_set_mode(CONSOLE_MODE_LINE);
}
