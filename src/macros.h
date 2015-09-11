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
#ifndef MACROS_H
#define MACROS_H

#define MACROS_NUM_MACROS 32

#include <string.h>
#include "eeprom.h"
#include "kbd.h"
#include "macro_types.h"
#include "reports.h"

typedef struct {
	uint8_t  scancode;
	uint8_t  modStates;
	uint8_t  modMask;
	uint8_t *addr;
} MacroDef;

extern MacroDef macrosDefs[MACROS_NUM_MACROS];

void macrosInit(void);
void macrosLoad(void);
void macrosPostProcessNKROReport(NKROReport *report, NKROReport *prevReport);

#endif
