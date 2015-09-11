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
#include "mcp4726.h"

uint16_t mcp4726Vref;

/*
 *
 */
void
mcp4726Init(void)
{
	twi_init();
}

/*
 *
 */
void
mcp4726SetVref(uint16_t vref)
{
	unsigned char twiData[2];

	mcp4726Vref = vref;

	twiData[0] =   (vref >> 8);
	twiData[0] &= ~(1 << 7);
	twiData[0] &= ~(1 << 6);
	twiData[0] &= ~(1 << 5);
	twiData[0] &= ~(1 << 4);

	twiData[1] = vref & 0xff;

	twi_write_data(twiData, 2);
}

/*
 *
 */
void
mcp4726StoreVref(void)
{
	eeprom_update_byte((uint8_t *)EEP_VREF_L, mcp4726Vref & 0xff);
	eeprom_update_byte((uint8_t *)EEP_VREF_H, mcp4726Vref >> 8);
}

/*
 *
 */
void
mcp4726LoadVref(void)
{
	uint16_t val;

	val =   eeprom_read_byte((uint8_t *)EEP_VREF_H);
	val <<= 8;
	val |=  eeprom_read_byte((uint8_t *)EEP_VREF_L);

	mcp4726SetVref(val);
}
