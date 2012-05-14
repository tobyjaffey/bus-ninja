ifeq ($(BOARD),ARDUINO)
include config-arduino.mk
else
ifeq ($(BOARD),TEENSY)
include config-teensy.mk
else
ifeq ($(BOARD),LEONARDPIRATE)
include config-leonard_pirate.mk
else
# Default to arduino
include config-arduino.mk
endif
endif
endif

include rules.mk

TARGET=ninja

CC=avr-gcc
SIZE=avr-size
OBJCOPY=avr-objcopy

F_CLOCK = $(F_CPU)

CFLAGS += -DVERSION_STR="\"$(shell date +"%d%m%y@%H:%M")-$(shell whoami)\""

SRCS +=  $(TARGET).c \
        cpu.c \
        parse.c \
        watchdog.c \
        hw_led.c \
        led.c \
        tick.c \
        console.c \
        commands.c \
        global_commands.c \
        bus.c \
        menu.c \

LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += -Wl,--relax
LDFLAGS += -Wl,--gc-sections

CFLAGS += -Os -mmcu=$(MCU) -Wall -DF_CPU=$(F_CPU) -DF_CLOCK=$(F_CLOCK)
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -fpack-struct
CFLAGS += -fshort-enums
#CFLAGS += --param inline-call-cost=2  
CFLAGS += -finline-limit=3 
CFLAGS += -fno-inline-small-functions 
CFLAGS += -Wall -Werror

CFLAGS += -DUSBMODE_DEFAULT=USBMODE_CDC

OBJ = $(SRCS:%.c=$(OBJDIR)/%.o)

OBJDIR= .

MCU_FLAG = $(shell $(SIZE) --help | grep -- --mcu > /dev/null && echo --mcu=$(MCU) )
FORMAT_FLAG = $(shell $(SIZE) --help | grep -- --format=.*avr > /dev/null && echo --format=avr )

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O ihex $(TARGET).elf $(TARGET).eep || exit 0

$(OBJDIR)/%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@ 


$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $^ --output $@ $(LDFLAGS)
	$(SIZE) $@
	$(SIZE) $(MCU_FLAG) $(FORMAT_FLAG) $(TARGET).elf

program: $(TARGET).hex
	$(PROGRAM_CMD)

clean:
	rm -f $(TARGET).hex $(TARGET).elf $(TARGET).eep $(TARGET).map $(OBJ)


