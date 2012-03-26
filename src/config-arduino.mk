#
# CONFIG FOR ARDUINO DIECEMILIA
# Compatible with Duemilanove
# 
MCU=atmega168
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
PROGRAM_CMD=avrdude -p m328p -c arduino -P/dev/ttyUSB0 -b57600 -D -Uflash:w:$(TARGET).hex


