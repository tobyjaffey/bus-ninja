#include "ninja.h"
#include "hw_spi.h"

void hw_spi_init(void)
{
    uint8_t tmp;

    SPI_DDR |= _BV(SPI_CS); // CS as output
    hw_spi_cs_deassert();

    // SCK and MOSI as output, SS as output - we're master
    SPI_DDR |= _BV(SPI_SCLK) | _BV(SPI_MOSI) | _BV(SPI_SS);

    // Enable MISO pull-up
    SPI_PORT |= _BV(SPI_MISO);

    // F_CPU    // div 128 @ 8Mhz = 62KHz
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1) | _BV(SPR0);

    // Clear status flags
    tmp = SPSR;
    tmp = SPDR;
}

void hw_spi_shutdown(void)
{
    // disable SPI
    SPCR &= ~ _BV(SPE);
}

// clock a byte in and out
uint8_t hw_spi_write8(uint8_t data)
{
    SPDR = data;
    while (!(SPSR & (1<<SPIF)));
    return SPDR;
}


