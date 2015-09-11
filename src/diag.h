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
#ifndef DIAG_H
#define DIAG_H

#include "descriptors.h"
#include "diag_types.h"
#include "exp.h"
#include "kbd.h"
#include "vref.h"

/* generic HID channel for diagnosis information and programming */

void diagReceiveReport(const uint8_t *reportData);
void diagFillReport(uint8_t *reportData, bool usingNKROReport);

#endif
