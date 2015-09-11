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
#include "dac101s101.h"

uint16_t dac101s101Vref;

/*
 *
 */
void
dac101s101Init(void)
{
	spiInit();
	spiDeselect();
}

/*
 *
 */
void
dac101s101SetVref(uint16_t vref)
{
	dac101s101Vref = vref;

	spiSelect();

	vref <<= 2;
	spi(vref >> 8);
	spi(vref &  0xff);

	spiDeselect();
}

/*
 *
 */
void
dac101s101StoreVref(void)
{
	eeprom_update_byte((uint8_t *)EEP_VREF_L, dac101s101Vref &  0xff);
	eeprom_update_byte((uint8_t *)EEP_VREF_H, dac101s101Vref >> 8);
}

/*
 *
 */
void
dac101s101LoadVref(void)
{
	uint16_t val;

	val =   eeprom_read_byte((uint8_t *)EEP_VREF_H);
	val <<= 8;
	val |=  eeprom_read_byte((uint8_t *)EEP_VREF_L);

	dac101s101SetVref(val);
}
