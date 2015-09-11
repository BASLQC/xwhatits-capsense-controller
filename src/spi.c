/******************************************************************************
  Copyright 2014 Tom Wong-Cornall

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.  
 ******************************************************************************/
#include "spi.h"

/*
 * SPI is on port B
 *
 * SS:      PB0
 * SCK:     PB1
 * MOSI:    PB2
 * MISO:    PB3
 */
void
spiInit(void)
{
	/* set SCK, SS, MOSI as outputs */
	DDRB |= (1 << PB1);
	DDRB |= (1 << PB0);
	DDRB |= (1 << PB2);

	/* init interface */
	SPCR =
	   (1 << SPE)  |               // enable SPI
	   (1 << MSTR) |               // master mode
	   (0 << CPOL) | (1 << CPHA) | // spi mode 1
	   (0 << SPR0) | (0 << SPR1)   // (clock / 4)
	   ;
	SPSR |= (1 << SPI2X); // double speed for (clock / 2)
}

/*
 * Do read/write. Macro spiRead just does this with dummy byte.
 */
uint8_t
spi(uint8_t data)
{
	/* put data to send into SPI data register */
	SPDR = data;

	/* wait for transmission complete */
	while (!(SPSR & (1 << SPIF)))
		;

	/* return data read from SPI */
	return SPDR;
}
