#ifndef GLOBAL_COMMANDS_H
#define GLOBAL_COMMANDS_H 1

#include "ninja.h"
#include <avr/pgmspace.h>

typedef BOOL (*global_command_func_t)(const uint8_t *tok_start, const uint8_t *line_end);

typedef struct
{
    const prog_char *name;
    global_command_func_t handler;
} global_command_t;

extern const global_command_t global_command_table[] PROGMEM;

/****************************************************************************************/
/* FIXME these macros will only work for command names which are valid C function names */

#define DECLARE_COMMAND(X) \
        const char X##Str[] PROGMEM = #X; \
        static BOOL global_command_##X(const uint8_t *tok_start, const uint8_t *line_end);

#define DEFINE_COMMAND(X) \
    static BOOL global_command_##X(const uint8_t *tok_start, const uint8_t *line_end)

#define ADD_COMMAND(X) \
    { X##Str, global_command_##X },

/****************************************************************************************/

#endif

