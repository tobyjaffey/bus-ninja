#
# CONFIG OPTIONS for features in development (use at your own risk)
#

# USB eStick-ish driver + JTAG PORTB driver + app
# eStick OpenOCD support, BEWARE buffer size in openocd/estick.c needs shrinking
#CONFIG_USBDEV_JTAG=y
#CONFIG_JTAG=y

# USBasp compatible AVR in-system-programmer
# FIXME TBD needs to be hooked into hw_spi and protocol completed
#CONFIG_USBDEV_AVRISP=y

# Hardware UART driver
#CONFIG_HW_UART=y

# Console on hw_uart
#CONFIG_HW_UART_CONSOLE=y

# Dump key codes
#CONFIG_COMMAND_DUMPKEYS=y

# FIXME TBD Bus Pirate like bitbang mode
#CONFIG_COMMAND_BITBANG=y

# Interactive menu example
#CONFIG_COMMAND_MENUTEST=y

