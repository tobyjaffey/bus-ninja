#ifndef PARSE_H
#define PARSE_H 1

#include "ninja.h"

/******************************************************************/

BOOL parse_number(const uint8_t *str, uint8_t len, uint32_t *result);

/******************************************************************/

BOOL tok(uint8_t const **tok_start, uint8_t const **tok_end, const uint8_t *line_end);
BOOL tok_num(const uint8_t **tok_start, const uint8_t **tok_end, const uint8_t *line_end, uint32_t *i);

#endif


