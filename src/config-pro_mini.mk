#
# CONFIG FOR ARDUINO PRO MINI
#

#F_CPU =  8000000UL
F_CPU = 16000000UL

# Switch on led on start
CONFIG_HW_LED_ALIVE_CHECK=y
# Is this working?
#CONFIG_DEBUG_RESET_REASON=n

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


#AVRDUDE_CFG = /etc/avrdude/avrdude.conf
AVRDUDE_CFG = /etc/avrdude.conf
AVRDUDE_PROGRAMMER = usbasp
AVRDUDE_PORT = usb
# baudrate is ignored for USB
AVRDUDE_BAUD = 115200


########################################

# -D = Disable auto erase for flash. Good for xmega
#AVRDUDE_OPTS = -D

AVRDUDE_CPU = m328p
MCU = atmega328p

CFLAGS += -DLED0_PORT=PORTB -DLED0_PIN=PB5 -DLED0_DDR=DDRB

PROGRAM_CMD=/usr/bin/avrdude -C $(AVRDUDE_CFG) -p$(AVRDUDE_CPU) -c$(AVRDUDE_PROGRAMMER) -P$(AVRDUDE_PORT) -b$(AVRDUDE_BAUD) $(AVRDUDE_OPTS) -Uflash:w:$(TARGET).hex
