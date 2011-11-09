/*
 * LED sequences (flashing, blinking, fading etc)
 * Linearly interpolates brightness values over time
 */

#include <avr/pgmspace.h>

#include "ninja.h"
#include "tick.h"
#include "hw_led.h"
#include "led.h"

/*******************************************************/

// The number of ticks in a step 
#define STEP_TIME   (TICKS_PER_SEC/256)

/*******************************************************/

/*
Each sequence is in the form:

Number of steps,
First step,
Second step,
...

Where each step is a command to fade between two values over a period:
Duration, Starting brightness value, Ending brightness value

The whole data block looks like:
{ NUM_STEPS, STEP0_DURATION, STEP0_START, STEP0_END, STEP1_DURATION, STEP1_START, STEP1_END, ... }
*/

static const uint8_t led_seq_off[] PROGMEM = 
{
    1,
    255, 0x00, 0x00,
};

static const uint8_t led_seq_on[] PROGMEM =
{
    1,
    255, 0xFF, 0xFF,
};

static const uint8_t led_seq_slowflash[] PROGMEM = 
{
    2,
    255, 0x00, 0x00,
    255, 0xFF, 0xFF
};

static const uint8_t led_seq_fastflash[] PROGMEM =
{
    2,
    32, 0x00, 0x00,
    32, 0xFF, 0xFF
};

static const uint8_t led_seq_blink[] PROGMEM = 
{
    2,
    255, 0x00, 0x00,
    16, 0xFF, 0xFF,
};

static const uint8_t led_seq_sine[] PROGMEM =
{
    8,
    16, 0x80, 0xD9,
    16, 0xD9, 0xFF,
    16, 0xFF, 0xD9,
    16, 0xD9, 0x80,
    16, 0x80, 0x27,
    16, 0x27, 0x00,
    16, 0x00, 0x27,
    16, 0x27, 0x80
};

static const uint8_t led_seq_glow[] PROGMEM = 
{
    2,
    64, 0xFF, 0x80,
    64, 0x80, 0xFF,
};


static const uint8_t *led_sequences[] PROGMEM =
{
    [LED_SEQ_OFF] = led_seq_off,
    [LED_SEQ_ON] = led_seq_on,
    [LED_SEQ_SLOWFLASH] = led_seq_slowflash,
    [LED_SEQ_FASTFLASH] = led_seq_fastflash,
    [LED_SEQ_BLINK] = led_seq_blink,
    [LED_SEQ_SINE] = led_seq_sine,
    [LED_SEQ_GLOW] = led_seq_glow,
};

static const uint8_t *cur_seq;

/*******************************************************/

static uint8_t seq_index = 0;   // index of 3 byte triple in sequence

static uint32_t step_timer = 0;
static uint8_t t;
static uint16_t seq_interp_A, seq_interp_B, seq_dur;

/*******************************************************/

static void led_seq_next(void)
{
    uint8_t seq_len;

    t = 0;
    seq_len =       pgm_read_byte(cur_seq);
    seq_dur =       pgm_read_byte(cur_seq + 1 + seq_index*3);
    seq_interp_A =  pgm_read_byte(cur_seq + 2 + seq_index*3);
    seq_interp_B =  pgm_read_byte(cur_seq + 3 + seq_index*3);
    seq_index =     (seq_index + 1) % seq_len;
}

void led_set_seq(uint8_t seq)
{
    cur_seq = (const uint8_t *)pgm_read_word(&led_sequences[seq]);  // lookup the sequence
    seq_index = 0;  // start from beginning of the sequence
    led_seq_next();
}

static void led_seq_tick(void)
{
    if (t >= seq_dur)
        led_seq_next();
    else
    {
        hw_led_set((seq_interp_A*(seq_dur-t)+seq_interp_B*t)/seq_dur);
        t++;
    }
}

void led_init(void)
{
    led_set_seq(LED_SEQ_OFF);
    led_seq_next();
}

void led_tick(uint32_t ticks)
{
    if (ticks > step_timer)
    {
        led_seq_tick();
        step_timer = tick_get() + STEP_TIME;
    }
}

