#include "ninja.h"
#include "console.h"
#include "bus.h"
#include "hw_i2c.h"

#include "bus_i2c.h"

/*
 * Because all buy the last rx must have an ACK, we delay each transaction by storing the state and data
 */

enum
{
    STATE_INIT,                 // starting state
    STATE_ADDR_PENDING,         // waiting for address byte
    STATE_START_PENDING,        // send address byte, waiting for data byte
    STATE_WRITE_DATA_PENDING,   // send data byte, waiting for data byte
    STATE_READ_DATA_PENDING,    // receive data, waiting for more data
    STATE_ERROR                 // null state, to stop any more bus traffic until state is reset
};

static uint8_t state;
static uint8_t pending_data;

static void bus_i2c_open(void)
{
    bus_log_puts_P(PSTR("I2C OPEN"));
    bus_log_newline();

    state = STATE_INIT;

    hw_i2c_init();
    hw_i2c_bus_clear();
}

static void bus_i2c_close(void)
{
    hw_i2c_shutdown();
}

static void bus_i2c_start(void)
{
    state = STATE_ADDR_PENDING;
}

static void bus_i2c_stop(void)
{
    uint8_t c;

    switch(state)
    {
        case STATE_START_PENDING:
            if (!hw_i2c_master_start(pending_data, pending_data & 0x1 ? hw_i2c_rd : hw_i2c_wr))
                bus_log_puts_P(PSTR("NACK"));
            else
                bus_log_puts_P(PSTR("OK"));

            bus_log_newline();
            hw_i2c_master_stop();
            bus_log_puts_P(PSTR("I2C STOP CONDITION"));
            bus_log_newline();
            break;

        case STATE_WRITE_DATA_PENDING:
            bus_log_puts_P(PSTR("I2C WRITE: "));
            bus_log_put0x8(pending_data);
            bus_log_puts_P(PSTR(" GOT ACK: "));

            if (hw_i2c_master_tx(&pending_data, 1)) // write the last byte
                bus_log_puts_P(PSTR("YES"));
            else
                bus_log_puts_P(PSTR("NO"));

            bus_log_newline();
            hw_i2c_master_stop();
            bus_log_puts_P(PSTR("I2C STOP CONDITION"));
            bus_log_newline();
            break;

        case STATE_READ_DATA_PENDING:
            bus_log_puts_P(PSTR("I2C READ: "));
            if (hw_i2c_master_rx(&c, 1, FALSE))  // last read
                bus_out(c);
            else
                bus_log_puts_P(PSTR("NO"));
            bus_log_newline();
            hw_i2c_master_stop();
            bus_log_puts_P(PSTR("I2C STOP CONDITION"));
            bus_log_newline();
            break;
    }

    state = STATE_INIT;
}

static void bus_i2c_write(uint8_t c)
{
    switch(state)
    {
        case STATE_ADDR_PENDING:
            pending_data = c;   // store address for later
            state = STATE_START_PENDING;
            break;

        case STATE_START_PENDING:
            bus_log_puts_P(PSTR("I2C START CONDITION"));
            bus_log_newline();
            bus_log_puts_P(PSTR("I2C WRITE: "));
            bus_log_put0x8(pending_data);
            bus_log_puts_P(PSTR(" GOT ACK: "));

            if (!hw_i2c_master_start(pending_data, pending_data & 0x1 ? hw_i2c_rd : hw_i2c_wr))
            {
                hw_i2c_bus_clear();
                bus_log_puts_P(PSTR("NO"));
                state = STATE_ERROR;
            }
            else
            {
                bus_log_puts_P(PSTR("YES"));

                pending_data = c;
                state = STATE_WRITE_DATA_PENDING;
            }
            bus_log_newline();
            break;

        case STATE_WRITE_DATA_PENDING:
            bus_log_puts_P(PSTR("I2C WRITE: "));
            bus_log_put0x8(pending_data);
            bus_log_puts_P(PSTR(" GOT ACK: "));

            if (hw_i2c_master_tx(&pending_data, 1))  // more data to come
            {
                bus_log_puts_P(PSTR("YES"));
                pending_data = c;
            }
            else
            {
                hw_i2c_bus_clear();
                bus_log_puts_P(PSTR("NO"));
                state = STATE_ERROR;
            }
            bus_log_newline();
            break;

        default:
            hw_i2c_bus_clear();
            state = STATE_ERROR;
            break;
    }
}

static void bus_i2c_read(void)
{
    uint8_t c;

    switch(state)
    {
        case STATE_START_PENDING:
            if (!hw_i2c_master_start(pending_data, pending_data & 0x1 ? hw_i2c_rd : hw_i2c_wr))
            {
                hw_i2c_bus_clear();
                bus_log_puts_P(PSTR("NACK"));
                state = STATE_ERROR;
            }
            else
            {
                bus_log_puts_P(PSTR("I2C START CONDITION"));
                state = STATE_READ_DATA_PENDING;
            }
            bus_log_newline();
            break;

        case STATE_READ_DATA_PENDING:
            bus_log_puts_P(PSTR("I2C READ: "));
            if (hw_i2c_master_rx(&c, 1, TRUE))  // more data to come
                bus_out(c);
            else
            {
                hw_i2c_bus_clear();
                bus_log_puts_P(PSTR("NO"));
                state = STATE_ERROR;
            }
            bus_log_newline();
            break;

        default:
            hw_i2c_bus_clear();
            state = STATE_ERROR;
            break;
    }
}

static BOOL bus_i2c_command(const uint8_t *start, size_t len, uint32_t repeat)
{
    uint8_t i;
    // address scanner
    if (len == 1 && *start == 's')  // FIXME
    {
        i = 0xFF;
        do
        {
            if (hw_i2c_master_start(i, i & 0x1))
            {
                bus_log_put0x8(i);
                bus_log_putc('(');
                bus_log_put0x8(i>>1);
                bus_log_putc(' ');
                if (i & 0x1)
                    bus_log_putc('R');
                else
                    bus_log_putc('W');
                bus_log_putc(')');

                bus_log_newline();
            }
            hw_i2c_bus_clear();
        }
        while(i-- > 0);
        return TRUE;
    }
    return FALSE;
}

const bus_interface_t bus_i2c =
{
    .open       = bus_i2c_open,
    .close      = bus_i2c_close,
    .start      = bus_i2c_start,
    .stop       = bus_i2c_stop,
    .write      = bus_i2c_write,
    .read       = bus_i2c_read,
    .command    = bus_i2c_command
};

