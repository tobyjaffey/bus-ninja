#ifndef HW_SPI_H
#define HW_SPI_H 1

#include <avr/io.h>

#if __AVR_ATmega168__
#define SPI_PORT    PORTB
#define SPI_DDR     DDRB
#define SPI_SS      PB2
#define SPI_MOSI    PB3
#define SPI_MISO    PB4
#define SPI_SCLK    PB5
#define SPI_CS      PB1
#elif __AVR_AT90USB162__
#define SPI_PORT    PORTB
#define SPI_DDR     DDRB
#define SPI_SS      PB0
#define SPI_SCLK    PB1
#define SPI_MOSI    PB2
#define SPI_MISO    PB3
#define SPI_CS      PB4
#elif __AVR_ATmega32U4__
#define SPI_PORT    PORTB
#define SPI_DDR     DDRB
#define SPI_SS      PB0
#define SPI_SCLK    PB1
#define SPI_MOSI    PB2
#define SPI_MISO    PB3
#define SPI_CS      PB4
#else
#error Unsupported processor
#endif

#define hw_spi_cs_assert()      (SPI_PORT) &= ~_BV(SPI_CS);    // active low
#define hw_spi_cs_deassert()    (SPI_PORT) |= _BV(SPI_CS);

void hw_spi_init(void);
void hw_spi_shutdown(void);
uint8_t hw_spi_write8(uint8_t data);

#endif

