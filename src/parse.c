#include "ninja.h"
#include "parse.h"

/*******************************************************************************/

// hex nibble to int
static uint8_t digit_to_int(uint8_t ch)
{
    uint8_t r;
    if (ch >= 'a')
        r = 'a' - 10;
    else
    {
        if (ch >= 'A')
            r = 'A' - 10;
        else
        {
            if (ch <= '9')
                r = '0';
            else
                r = 0;
        }
    }

    return ch - r;
}

// parse binary, hex and decimal numbers
BOOL parse_number(const uint8_t *str, uint8_t len, uint32_t *result)
{
    uint8_t base = 10;
    uint8_t i;
    uint8_t c;
    uint8_t digit;

    *result = 0;

    for (i=0;i<len;i++)
    {
        c = str[i];

        if (*result == 0)
        {
            if ((i == 0 || i == 1) && c == 'b')     // 0b/b for binary
            {
                if (len < 2)
                    return FALSE;
                base = 2;
                continue;
            }
            else
            if (i==0 && c == 'h')       // h for hex
            {
                if (len < 2)
                    return FALSE;
                base = 16;
                continue;
            }
            else
            if (i == 1 && c == 'x')     // 0x for hex
            {
                if (len < 3)
                    return FALSE;
                base = 16;
                continue;
            }
        }

        digit = digit_to_int(c);

        if (digit < base)
            *result = (*result) * base + digit;
        else
            return FALSE;
    }
    return TRUE;
}

/*******************************************************************************/

// find the next token in the buffer given by *tok_start to line_end
// on success, returns TRUE, *tok_start points at first byte of found token, *tok_end points at first character after end of token
BOOL tok(uint8_t const **tok_start, uint8_t const **tok_end, const uint8_t *line_end)
{
    // skip whitespace on front
    while(*tok_start != line_end)
    {
        if (**tok_start == ' ' || **tok_start == '\t')
            (*tok_start)++;
        else
            break;
    }
    // *tok_start is now first non-whitespace character in token

    *tok_end = *tok_start;
    // skip to end of token
    while(*tok_end != line_end)
    {
        if (**tok_end == ' ' || **tok_end == '\t')
            break;
        else
            (*tok_end)++;
    }
    // *tok_end is now first whitespace character after token or line_end

    return *tok_end != *tok_start;
}

// advance tok_start/tok_end to next token and read number value into i
BOOL tok_num(const uint8_t **tok_start, const uint8_t **tok_end, const uint8_t *line_end, uint32_t *i)
{
    if (tok(tok_start, tok_end, line_end))
    {
        if (parse_number(*tok_start, (*tok_end)-(*tok_start), i))
        {
            *tok_start = *tok_end;  // skip past token
            return TRUE;
        }
        else
        {
//            *tok_start = *tok_end;  // skip past token
        }
    }

    return FALSE;
}

/*******************************************************************************/

