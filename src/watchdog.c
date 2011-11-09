#include "ninja.h"
#include "watchdog.h"

#include <avr/io.h>
#include <avr/wdt.h>

void watchdog_init(void)
{
#ifdef CONFIG_WATCHDOG
	MCUSR |= (1 << WDRF);
    wdt_enable(WDTO_30MS);
#else
	MCUSR &= ~(1 << WDRF);
    wdt_disable();
#endif
}

void watchdog_reset(void)
{
#ifdef CONFIG_WATCHDOG
    wdt_reset();
#endif
}

