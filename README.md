# Bus Ninja

## Bus Ninja
A Bus Pirate clone for Atmel AVR microcontrollers, including the Arduino and Teensy. Featuring a serial console, I2C, SPI and more. On the USB enabled AVRs (AT90USBxxxx and ATMEGAxxUx) USB over serial is used, allowing for single chip Bus Ninja hardware.

Bus Ninja lets you quickly protoype with I2C and SPI devices without writing any code. Just wire up a device to your board and start talking to it directly from the Bus Ninja console.

### Features

- Interactive shell with Bus Pirate like syntax
- I2C (soft i2c driver from userial)
- I2C address scanner ('s' command from i2c mode)
- SPI
- Flow controlled USB CDC (serial) (AT90USBxxxx/ATMEGAxxUx only)
- Modular software making it straightforward to add
 - New commands
 - New buses
 - New USB gadgets - mass storage EEPROM interface, HID devices, etc. (AT90USBxxxx/ATMEGAxxUx only)

### Commands
#### help
Print list of available commands

#### version
Print build string

#### reset
Reset board

#### spi
Enter SPI bus mode

#### i2c
Enter i2c bus mode

#### led <0-6>
Set LED pattern

### Wiring
When connecting up I2C and SPI devices, signals should be connected the pins below. For Teensy, use the AT90USBxxxx variants.

<table>
<tr><th>Function</th><th>Arduino pin</th><th>AT90USBxxxx/ATMEGAxxUx pin</th</tr>
<tr><td>I2C SDA</td><td>Analog 4 (PORTC4)</td><td>PORTB2</td></tr>
<tr><td>I2C SCL</td><td>Analog 5 (PORTC5)</td><td>PORTB1</td></tr>
<tr><td>SPI CLK</td><td>Digital 13 (PORTB5)</td><td>PORTB1</td></tr>
<tr><td>SPI MISO</td><td>Digital 12 (PORTB4)</td><td>PORTB3</td></tr>
<tr><td>SPI MOSI</td><td>Digital 11 (PORTB3)</td><td>PORTB2</td></tr>
<tr><td>SPI CS</td><td>Digital 9 (PORTB1)</td><td>PORTB4</td></tr>
<tr><td>LED</td><td>Digital 2 (PORTD2)</td><td>PORTD4</td></tr>
</table>


### Bus commands
Once a bus mode is selected (`i2c`/`spi`), bus commands can be sent. See the Bus Pirate manuals</a> for more details

Eg.
```c
    > spi
    > [0x40 0x0A 0x28]
    CS ENABLED
    WRITE: 0x40
    WRITE: 0x0A
    WRITE: 0x28
    CS DISABLED
```
(see also the command examples in the <a href="test">test/</a> directory)


### Arduino Examples

#### Controlling an LED
To prove that the Bus Ninja software is working on your Arduino, start by controlling an LED. Connect the anode of your LED to digital pin 2 and the cathode to ground (it would be wise to also add a current limiting resistor in series, say 220R).

Connect to the Arduino's serial port with your favourite terminal emulator at 8-N-1 9600bps.
```c
    screen /dev/ttyUSB0 9600
```
Type `led` followed by an integer from 0-6 to set the LED fade pattern.

#### Controlling a Microchip MCP23S17 SPI 16-bit I/O expander
Wire the chip to the Arduino like this:

<table>
<tr><th>MCP23S17</th><th>Arduino</th></tr>
<tr><td>VDD</td><td>5V</td></tr>
<tr><td>VSS</td><td>GND</td></tr>
<tr><td>SO</td><td>SPI MISO (Digital 12)</td></tr>
<tr><td>SI</td><td>SPI MOSI (Digital 11)</td></tr>
<tr><td>CS</td><td>SPI CS (Digital 9)</td></tr>
<tr><td>SCK</td><td>SPI CLK (Digital 13)</td></tr>
<tr><td>Reset</td><td>5V</td></tr>
</table>

Connect an LED, voltmeter or oscilloscope to GPA0. Then, open the Bus Ninja serial console and enter commands.

Enter SPI mode
```    
    spi

Initialise the chip for non-sequential access
    [0x40 0x0A 0x28]    # WR_REG(0) IOCONA      SEQ_OFF|HAEN

Set all of PORTA as outputs
    [0x40 0x00 0x00]    # WR_REG(0) GPIOADIR    ALL_OUTPUT

Set GPIOA-0 high
    [0x40 0x12 0x01]    # WR_REG(0) GPIOA       GPIOA-0

Set GPIOA-0 low
    [0x40 0x12 0x00]    # WR_REG(0) GPIOA       GPIOA-0
```

#### Controlling a Microchip 24LC16B I2C EEPROM
Wire the chip to the Arduino like this:

<table>
<tr><th>24LC16B</th><th>Arduino</th></tr>
<tr><td>VCC</td><td>5V</td></tr>
<tr><td>VSS</td><td>GND</td></tr>
<tr><td>SDA</td><td>I2C SDA (Analog 4)</td></tr>
<tr><td>SI</td><td>I2C SCL (Analog 5)</td></tr>
<tr><td>Reset</td><td>5V</td></tr>
</table>

For I2C to properly function you must add two pullup resistors. One from SDA to 5V, one from SCL to 5V.

Enter I2C mode
    i2c

Write {0x10, 0x20, 0x30, 0x40} at address 0x0000
    [0xA6 0 0 0x10 0x20 0x30 0x40]

Set the read pointer to address 0x0000
    [0xA6 0 0]

Read back your 4 bytes
    [0xA7 r:4]


### Building the code
Edit `config.mk` and change `PROGRAM_CMD` for your system.

To build and flash to Arduino Diecemilia (atmega168):
    cd src && make BOARD=ARDUINO clean &amp;&amp; make BOARD=ARDUINO &amp;&amp; make BOARD=ARDUINO program

To build for Teensy 1.0 (at90usb162):
    cd src && make BOARD=TEENSY clean && make BOARD=TEENSY && make BOARD=TEENSY program


### FAQ

Q. Why is it called Bus Ninja?
A. Because Ninjas are better than Pirates and Yarrrrrduino sounds silly.

Q. What hardware does Bus Ninja run on?
A. It was originally developed on Teensy1 (AT90USB162), then later ported to Arduino (ATMega168). Adding support for other AVRs should just be a matter of changing the definitions in config.mk.

Q. Why do I lose characters when I paste to the Arduino?
A. The Arduino serial port isn't flow controlled, use the USB version of Bus Ninja

Q. How do I port Bus Ninja to my custom AVR/Arduino board?
A. Edit config.mk, setup FCPU, MCU, LED PORT/PIN and desired features.

Q. Why aren't you using LUFA's scheduler/task framework?
A. Bus Ninja needs to remain portable to other stacks and processors, so it doesn't use LUFA's application framework.

Q. How do I add a new command?
A. Use the macros DECLARE_COMMAND(mycmd), ADD_COMMAND(mycmd) and DEFINE_COMMAND(mycmd) in global_commands.c

Q. How do I add a new bus?
A. Declare a `bus_interface_t`, provide handlers for the methods, enable it with `bus_init(&my_bus)`. See `bus_spi.[ch]`. The buses are enabled by a command in `global_commands.c`

Q. How do I disable a feature?
A. Edit config.mk, comment out unused features.

Q. Why isn't the LED command hooked up to the on-board LED on Arduino?
A. The LED is on the SPI CLK line, so is needed for SPI.

Q. Why clone the Bus Pirate?
A. To get to a single chip Bus Pirate work-a-like with a free software toolchain.


### License and acknowledgements
Unless otherwise stated, everything is licensed under CC-0.

Bus Ninja also contains code from:
    LUFA (Dean Camera)
    userial (Thomas Pircher)
    estick-jtag/opendous-jtag (Cahya Wirawan, Vladimir Fonov, Dean Camera, Denver Gingerich)

Although Bus Ninja shares no code with the Bus Pirate project, it wouldn't exist without it. Thank you.

