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
#ifndef SCAN_H
#define SCAN_H

#include <util/delay_basic.h>
#include "kbd.h"
#include "sr.h"

/* looks backwards, but only because LM339 outputs are open-collector */
#if defined(KBD_ACTIVE_LOW)
#	define KBD_ROW_TEST(PINx, Pxn) (PINx & (1 << Pxn))
#elif defined(KBD_ACTIVE_HIGH)
#	define KBD_ROW_TEST(PINx, Pxn) (!(PINx & (1 << Pxn)))
#endif

/* when changing these numbers, remember counting starts from 0 */
#define SCAN_DB_THRESH 3
#define SCAN_DB_TOP    7

extern volatile uint8_t scanTick;
extern volatile int8_t  scanState[KBD_COLS][KBD_ROWS];

void scanInit(void);
void scanEnable(void);
void scanDisable(void);
void scanPause(void);
void scanResume(void);

#endif
