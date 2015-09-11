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
#ifndef VREF_H
#define VREF_H

#include "dac101s101.h"
#include "mcp4726.h"

#ifndef BOARD_REV
#	error "BOARD_REV not specified in makefile"
#endif

#if defined(BEAMSPRING) && BOARD_REV <= 2
#	define vrefInit()	mcp4726Init()
#	define vrefSet(x)	mcp4726SetVref(x)
#	define vrefStore()	mcp4726StoreVref()
#	define vrefLoad()	mcp4726LoadVref()
static inline uint16_t vrefGet(void) { return mcp4726Vref; }
#	define VREF_MAX 4095
#elif (defined(BEAMSPRING) && \
       BOARD_REV <= 4) || defined(BEAMSPRING_DISPLAYWRITER) || defined(MODEL_F)
#	define vrefInit() dac101s101Init()
#	define vrefSet(x) dac101s101SetVref(x)
#	define vrefStore() dac101s101StoreVref()
#	define vrefLoad() dac101s101LoadVref()
static inline uint16_t vrefGet(void) { return dac101s101Vref; }
#	define VREF_MAX 1023
#else
#	error "BOARD_TYPE not specified in makefile"
#endif

#endif
