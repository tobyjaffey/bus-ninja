#ifndef TICK_H
#define TICK_H 1

#include "ninja.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define TICKS_PER_SEC ((F_CPU) / 256)

void tick_init(void);
uint32_t tick_get(void);

#endif

