/*
 * Set the LED's brightness, using Binary Code Modulation
 */

#include "ninja.h"
#include "hw_led.h"

static uint8_t hw_led_brightness = 0xFF;

void hw_led_set(uint8_t v)
{
    hw_led_brightness = v;
}

/* Called from tick timer ISR */
void hw_led_tick(uint32_t ticks)
{
    ticks = ticks & 0xFF;

    switch(ticks)
    {
        case 0x1:
        case 0x2:
        case 0x4:
        case 0x8:
        case 0x10:
        case 0x20:
        case 0x40:
        case 0x80:
            if (hw_led_brightness & ticks)
                ENABLE_LED0;
            else
                DISABLE_LED0;
    }

}

