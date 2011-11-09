#ifndef USBDEV_H
#define USBDEV_H 1

#include "ninja.h"

enum
{
    USBMODE_NULL,
    USBMODE_CDC,
#ifdef CONFIG_USBDEV_MASS_STORAGE
    USBMODE_MS,
#endif
#ifdef CONFIG_USBDEV_JTAG
    USBMODE_JTAG,
#endif
#ifdef CONFIG_USBDEV_AVRISP
    USBMODE_AVRISP,
#endif
};

void usbdev_init(uint8_t mode);
void usbdev_tick(void);

#endif

