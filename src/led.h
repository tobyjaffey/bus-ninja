#ifndef LED_H
#define LED_H 1

void led_init(void);
void led_tick(uint32_t ticks);

void led_set_seq(uint8_t seq);

enum
{
    LED_SEQ_OFF,
    LED_SEQ_ON,
    LED_SEQ_SLOWFLASH,
    LED_SEQ_FASTFLASH,
    LED_SEQ_BLINK,
    LED_SEQ_SINE,
    LED_SEQ_GLOW,

    LED_SEQ_MAX
};

#endif

