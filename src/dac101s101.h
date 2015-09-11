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
#ifndef DAC101S101_H
#define DAC101S101_H

#include "eeprom.h"
#include "spi.h"

extern uint16_t dac101s101Vref; // cached so we can query from diag interface

void dac101s101Init(void);
void dac101s101SetVref(uint16_t vref);
void dac101s101StoreVref(void);
void dac101s101LoadVref(void);

#endif
