#include "ninja.h"
#include "cpu.h"
#include "tick.h"
#include "hw_led.h"
#include "led.h"
#include "watchdog.h"
#include "console.h"
#include "bus.h"

#ifdef CONFIG_USBDEV
#include "usbdev.h"
#endif
#ifdef CONFIG_HW_UART
#include "hw_uart.h"
#endif

void driver_tick(void)
{
    watchdog_reset();
#ifdef CONFIG_HW_UART
    hw_uart_tick();
#endif
#ifdef CONFIG_USBDEV
    usbdev_tick();
#endif

    console_tick();
    led_tick(tick_get());
}

int main(void)
{
    /* Bring up processor */
	cpu_init();
    watchdog_init();

    /* Bring up low level LED */
    hw_led_init();
#ifdef CONFIG_HW_UART
    /* Bring up low level hardware UART */
    hw_uart_init();
#endif
    /* Setup timer for tick counting */
    tick_init();

    /* Bring up high level LED */
    led_init();

    /* Bring up console */
    console_init();

#ifdef CONFIG_DEBUG_RESET_REASON
    /* Report reason for last reset */
    console_puts_P(PSTR("Reset cause:"));
    console_puthex8(MCUSR);
    console_newline();
#endif

    /* Set default LED pattern */
    led_set_seq(LED_SEQ_SINE);

#ifdef CONFIG_USBDEV
    usbdev_init(USBMODE_DEFAULT);
#endif

    /* no bus mode selected by default */
    bus_init(NULL);

    /* Enable interrupts */
    cpu_enable_int();

    while(1)
	{
        driver_tick();
	}
}


