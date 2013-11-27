#include "ninja.h"
#include "cpu.h"

#include <avr/power.h>
#include <avr/wdt.h>

void cpu_init(void)
{
	/* XXX an associated macro is checked, not the function */
#ifdef clock_prescale_get
	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#endif
}

void cpu_reset(void)
{
    wdt_enable(WDTO_15MS);
    while(1);
}

