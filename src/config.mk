#################################################################
# Config options
# See rules.mk for the makefile fragments enabled by each option

#
# CONFIG FOR ARDUINO
# 
MCU=atmega168
F_CPU = 16000000UL
CFLAGS += -DLED0_PORT=PORTB -DLED0_PIN=PB5 -DLED0_DDR=DDRB
# Hardware watchdog
CONFIG_WATCHDOG=y
# I2C support
CONFIG_BUS_I2C=y
# SPI support
CONFIG_BUS_SPI=y
# Hardware UART driver
CONFIG_HW_UART=y
# Console on hw_uart
CONFIG_HW_UART_CONSOLE=y
PROGRAM_CMD=/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avrdude -C /Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/etc/avrdude.conf -pm168 -cstk500v1 -P/dev/tty.usbserial-A1001MTD -b19200 -D -Uflash:w:$(TARGET).hex


##
## CONFIG FOR TEENSY/MICROPENDOUS/AT90USBxxx etc
## 
#MCU=at90usb162
#F_CPU = 16000000UL
#CFLAGS += -DLED0_PORT=PORTD -DLED0_PIN=PD4 -DLED0_DDR=DDRD
## Hardware watchdog
#CONFIG_WATCHDOG=y
## I2C support
#CONFIG_BUS_I2C=y
## SPI support
#CONFIG_BUS_SPI=y
## USB CDC serial driver
#CONFIG_USBDEV_CDC=y
## Console on CDC serial
#CONFIG_USBDEV_CDC_CONSOLE=y
#PROGRAM_CMD=avrdude -F -p usb162 -c avrusb500 -P /dev/tty.usbserial-A300* -U flash:w:$(TARGET).hex -U lfuse:w:0xCE:m -U hfuse:w:0xD8:m -U efuse:w:0xFF:m



##################################################################

