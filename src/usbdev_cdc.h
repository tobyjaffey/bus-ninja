#ifndef USBDEV_CDC_H
#define USBDEV_CDC_H 1

#include "ninja.h"

typedef void (*usbdev_cdc_rx_cb_func_t)(uint8_t c);
typedef BOOL (*usbdev_cdc_rx_ready_cb_func_t)(void);


void usbdev_cdc_init(void);
void usbdev_cdc_shutdown(void);
void usbdev_cdc_tick(void);

void usbdev_cdc_putc(uint8_t c);
void usbdev_cdc_set_rx_cb(usbdev_cdc_rx_cb_func_t f);
void usbdev_cdc_set_rx_ready_cb(usbdev_cdc_rx_ready_cb_func_t f);

uint16_t usbdev_cdc_get_descriptor(const uint16_t wValue, const uint8_t wIndex, void** const DescriptorAddress);
void usbdev_cdc_device_configuration_changed(void);
void usbdev_cdc_unhandled_control_request(void);
void usbdev_cdc_device_connect(BOOL isConnected);


#endif

