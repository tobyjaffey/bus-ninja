#ifndef NINJA_H
#define NINJA_H 1

#include <stdint.h>
#include <stdlib.h>

#include <avr/pgmspace.h>


#include <stdbool.h>

#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif
#ifndef BOOL
typedef bool BOOL;
#endif

void driver_tick(void);

#endif

