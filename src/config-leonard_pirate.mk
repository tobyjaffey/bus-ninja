#
# CONFIG FOR TEENSY/MICROPENDOUS/AT90USBxxx etc
# 
MCU=atmega32u4
F_CPU = 16000000UL
CFLAGS += -DLED0_PORT=PORTC -DLED0_PIN=PC7 -DLED0_DDR=DDRC
# Hardware watchdog
CONFIG_WATCHDOG=y
# I2C support
CONFIG_BUS_I2C=y
# SPI support
CONFIG_BUS_SPI=y
# USB CDC serial driver
CONFIG_USBDEV_CDC=y
# Console on CDC serial
CONFIG_USBDEV_CDC_CONSOLE=y
PROGRAM_CMD=avrdude -p atmega32u4 -c stk500v2 -P usb -F -U flash:w:$(TARGET).hex -U lfuse:w:0xCE:m -U hfuse:w:0xD8:m -U efuse:w:0xFF:m


