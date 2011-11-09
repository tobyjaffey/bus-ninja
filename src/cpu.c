#include "ninja.h"
#include "cpu.h"

#include <avr/power.h>
#include <avr/wdt.h>

void cpu_init(void)
{
	/* Disable clock division */
	clock_prescale_set(clock_div_1);
}

void cpu_reset(void)
{
    wdt_enable(WDTO_15MS);
    while(1);
}

