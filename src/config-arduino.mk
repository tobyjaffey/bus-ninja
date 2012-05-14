#
# CONFIG FOR ARDUINO DIECEMILIA
# Compatible with Duemilanove
# 
MCU=atmega328p
F_CPU = 16000000UL
CFLAGS += -DLED0_PORT=PORTD -DLED0_PIN=PD2 -DLED0_DDR=DDRD
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
# Note: MCU type and speed for Duemilanove
PROGRAM_CMD=/usr/bin/avrdude -C /etc/avrdude/avrdude.conf -pm328 -cstk500v1 -P/dev/ttyUSB0 -b115200 -D -Uflash:w:$(TARGET).hex

