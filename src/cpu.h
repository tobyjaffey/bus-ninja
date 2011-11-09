#ifndef CPU_H
#define CPU_H 1

#include <avr/interrupt.h>

void cpu_init(void);
void cpu_reset(void);

#define cpu_enable_int()  sei()
#define cpu_disable_int() cli()

#endif

