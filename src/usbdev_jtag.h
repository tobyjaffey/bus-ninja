#ifndef USBDEV_JTAG_H
#define USBDEV_JTAG_H 1

#include "ninja.h"

void usbdev_jtag_init(void);
void usbdev_jtag_shutdown(void);
void usbdev_jtag_tick(void);

uint16_t usbdev_jtag_get_descriptor(const uint16_t wValue, const uint8_t wIndex, void** const DescriptorAddress);
void usbdev_jtag_device_configuration_changed(void);
void usbdev_jtag_unhandled_control_request(void);
void usbdev_jtag_device_connect(BOOL isConnected);

#endif

