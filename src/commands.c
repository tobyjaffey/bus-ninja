#include <avr/pgmspace.h>
#include "ninja.h"
#include "parse.h"
#include "commands.h"
#include "global_commands.h"
#include "console.h"
#include "bus.h"

/*****************************************************************************/

enum
{
    STATE_SEARCHING,
    STATE_IN_TOKEN,
};

/*****************************************************************************/

// "mycommand ... .... ..."
//  ^       ^            ^
//  cmdname |            |
//          cmdname+len  |
//                       line_end
static BOOL handle_global_command(const uint8_t *cmdname, size_t len, const uint8_t *line_end)
{
    const global_command_t *cmd_ptr = global_command_table;
    global_command_t cmd;

    memcpy_P(&cmd, cmd_ptr, sizeof(global_command_t));
    while(NULL != cmd.name)
    {
        if (memcmp_P(cmdname, cmd.name, len)==0 && strlen_P(cmd.name) <= len)
        {
            if (!(*cmd.handler)(cmdname+len, line_end))
            {
                console_puts_P(PSTR("Error"));
                console_newline();
            }
            return TRUE;
        }
        memcpy_P(&cmd, ++cmd_ptr, sizeof(global_command_t));
    }
    return FALSE;
}

/*****************************************************************************/

// returns TRUE if rest of line should be ignored
static BOOL handle_command(const uint8_t *start, const uint8_t *end, BOOL firstToken, const uint8_t *line_end)
{
    uint32_t num;
    const uint8_t *ptr = start;
    uint32_t repeat = 1;

    // handle repeat commands, A:5
    while(ptr != end)
    {
        if (*ptr == ':' && ptr-start > 0)   // found at least one character followed by ':'
        {
            if (parse_number(ptr+1, end - (ptr+1), &repeat))
            {
                end = ptr;    // discard the ":X" part
                break;
            }
            else
                repeat = 0; // parse_number is destructive
        }
        ptr++;
    }
    
    // handle write command (a number)
    if (parse_number(start, end-start, &num))
    {
        if (NULL != current_bus && NULL != current_bus->write)
        {
            while(repeat--)
                current_bus->write((uint8_t)num);
            return FALSE;
        }
    }
    else
    {
        if (end-start == 1 && (start[0] == 'r' || start[0] == 'R'))
        {   // handle read command
            if (NULL != current_bus && NULL != current_bus->read)
            {
                while(repeat--)
                    current_bus->read();
                return FALSE;
            }
        }
        else
        {   // handle other commands
            if (firstToken)
            {
                if (handle_global_command(start, end-start, line_end))
                    return TRUE;
            }
            if (NULL != current_bus && NULL != current_bus->command)
            {
                if (current_bus->command(start, end-start, repeat))
                    return FALSE;
                // else, nosuch command
            }
        }
    }

    console_puts_P(PSTR("BadCmd"));
    console_newline();

    return FALSE;
}

void execute_command_line(const uint8_t *str, size_t len)
{
    uint8_t state = STATE_SEARCHING;
    uint8_t c;
    const uint8_t *end = str+len;
    const uint8_t *token_start = str;
    BOOL firstToken = TRUE;

    while(str != end)
    {
        c = *str;

        switch(state)
        {
            case STATE_SEARCHING:
                switch(c)
                {
                    case '#':   // comment
                        return; // ignore the rest of the line

                    case ' ':   // whitespace
                    case '\t':
                    case ',':
                        break;

                    case '[':
                    case '{':
                        firstToken = FALSE;
                        if (NULL != current_bus && NULL != current_bus->start)
                            current_bus->start();
                        break;

                    case ']':
                    case '}':
                        firstToken = FALSE;
                        if (NULL != current_bus && NULL != current_bus->stop)
                            current_bus->stop();
                        break;

                    default:    // start of token
                        state = STATE_IN_TOKEN;
                        token_start = str;
                        break;
                }
                str++;  // next character
                break;  // end of case STATE_SEARCHING

            case STATE_IN_TOKEN:
                switch(c)
                {
                    case ' ':   // end of token
                    case '\t':
                    case ',':
                    case '[':
                    case '{':
                    case ']':
                    case '}':
                    case '#':
                        if (handle_command(token_start, str, firstToken, end))
                            return;
                        firstToken = FALSE;
                        state = STATE_SEARCHING;
                        break;

                    default:
                        // keep scanning token
                        str++;
                        break;
                }
        }
    }

    if (STATE_IN_TOKEN == state)
        handle_command(token_start, end, firstToken, end);
}



