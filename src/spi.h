/******************************************************************************
  Copyright 2014 Tom Cornall

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
#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

void    spiInit(void);
uint8_t spi(uint8_t data);
#define spiRead() (spi(0x00))
#define spiSelect() (PORTB &= ~(1 << PB0))
#define spiDeselect() (PORTB |= (1 << PB0))

#endif
