#ifndef USBDEV_AVRISP_H
#define USBDEV_AVRISP_H 1

#include "ninja.h"

void usbdev_avrisp_init(void);
void usbdev_avrisp_shutdown(void);
void usbdev_avrisp_tick(void);

uint16_t usbdev_avrisp_get_descriptor(const uint16_t wValue, const uint8_t wIndex, void** const DescriptorAddress);
void usbdev_avrisp_device_configuration_changed(void);
void usbdev_avrisp_unhandled_control_request(void);
void usbdev_avrisp_device_connect(BOOL isConnected);

#endif

