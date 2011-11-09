#ifndef HW_LED_H
#define HW_LED_H 1

#include <avr/io.h>

#define SETUP_LED0      (LED0_DDR) |= _BV(LED0_PIN)
#define DISABLE_LED0    (LED0_PORT) &= ~_BV(LED0_PIN)
#define ENABLE_LED0     (LED0_PORT) |= _BV(LED0_PIN)

// setup LEDs
#define hw_led_init() {SETUP_LED0;}

void hw_led_set(uint8_t v);
void hw_led_tick(uint32_t ticks);

#endif
