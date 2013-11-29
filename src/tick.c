#include "tick.h"
#include "hw_led.h"

static volatile uint32_t ticks = 0;

ISR(TIMER0_OVF_vect)
{
    ticks++;

#ifdef CONFIG_HW_LED_ALIVE_CHECK
    hw_led_tick(ticks);
#endif
}

void tick_init(void)
{
    // setup clock divider. Timer0 overflows on counting to 256
#if __AVR_ATmega8__
    TCCR0 |= _BV(CS00);
    TIFR  |= _BV(TOV0);

    TCNT0  = 0x0;

    // enable overflow interrupts
    TIMSK |= _BV(TOIE0);
#else
    TCCR0B |= _BV(CS00);
    TIFR0 |= _BV(TOV0);

    TCNT0 = 0x0;

    // enable overflow interrupts
    TIMSK0 |= _BV(TOIE0);
#endif
}

uint32_t tick_get(void)
{
    return ticks;
}

