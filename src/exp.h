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
#ifndef EXP_H
#define EXP_H

#include <avr/io.h>
#include "eeprom.h"
#include "exp_types.h"
#include "kbd.h"
#include "reports.h"

/* handling of 6-pin expansion header (P2) */

extern uint8_t expMode;
extern uint8_t expVal1;
extern uint8_t expVal2;

void expClear(void);
void expReset(void);
void expMSTick(void);
bool expProcessScan(bool scanChanged);
void expSetMode(uint8_t mode, uint8_t val1, uint8_t val2);
void expStore(void);
void expLoad(void);

#endif
