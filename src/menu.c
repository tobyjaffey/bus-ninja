#include "ninja.h"
#include "console.h"

#include "menu.h"

#if 1
/*****************************************************************/
// Define a menu, everything is boilerplate except for the X(SYMBOL, STRING) entries

#undef MENU 
#define MENU \
    X(MY_MENU_RAT, "rat") \
    X(MY_MENU_CAT, "cat") \
    X(MY_MENU_DOG, "dog") 

#undef X
#define X(sym,str) const uint8_t sym##_str[] PROGMEM = str;
MENU
#undef X
#define X(sym,str) sym,
enum { MENU };
#undef X
#define X(sym,str) sym##_str,
const uint8_t *my_menu[] PROGMEM = { MENU NULL };
#endif
/*****************************************************************/
#if 1
/*****************************************************************/
// Define a menu, everything is boilerplate except for the X(SYMBOL, STRING) entries

#undef MY_MENU2 
#define MY_MENU2 \
    X(PIG, "pig") \
    X(BAT, "bat") \
    X(UNIVERSE, "whole universe") 

#undef X
#define X(sym,str) const uint8_t sym##_str[] PROGMEM = str;
MY_MENU2
#undef X
#define X(sym,str) sym,
enum { MY_MENU2 };
#undef X
#define X(sym,str) sym##_str,
const uint8_t *my_menu2[] PROGMEM = { MY_MENU2 NULL };

/*****************************************************************/
#endif

static void menu_print_item(uint8_t index, const uint8_t *str_P)
{
    console_putdec(index);
    console_putc('.');
    console_putc(' ');
    console_puts_P((const char *)str_P);
    console_newline();
}

uint8_t menu_show(const uint8_t **menu_P)
{
    const uint8_t *str_P;
    uint8_t index = 0;

    while((str_P = (const uint8_t *)pgm_read_word(menu_P)) != NULL)
    {
        menu_print_item(index++, str_P);
        menu_P++;
    }

    return index;
}

uint8_t menu_pick_key(uint8_t max)
{
    uint8_t c;

    console_set_mode(CONSOLE_MODE_KEY);

    do
    {
        while(!console_key_poll(&c))
            driver_tick();
    }
    while(c < '0' || c >= '0' + max);

    console_putc(c);
    console_newline();

    console_set_mode(CONSOLE_MODE_LINE);

    return c - '0';
}

void mymenu(void)
{
    menu_pick_key(menu_show(my_menu2));
    switch(menu_pick_key(menu_show(my_menu)))
    {
        case MY_MENU_RAT:
            console_puts_P(PSTR("picked rat"));
            break;
        case MY_MENU_DOG:
            console_puts_P(PSTR("picked dog"));
            break;
        case MY_MENU_CAT:
            console_puts_P(PSTR("picked cat"));
            break;
    }
}
