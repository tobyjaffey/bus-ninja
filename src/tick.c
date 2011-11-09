#include "tick.h"
#include "hw_led.h"

static volatile uint32_t ticks = 0;

ISR(TIMER0_OVF_vect)
{
    ticks++;

    hw_led_tick(ticks);
}

void tick_init(void)
{
    // setup clock divider. Timer0 overflows on counting to 256
    TCCR0B |= _BV(CS00);
    TIFR0 |= _BV(TOV0);

    TCNT0 = 0x0;

    // enable overflow interrupts
    TIMSK0 |= _BV(TOIE0);
}

uint32_t tick_get(void)
{
    return ticks;
}

