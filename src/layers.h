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
#ifndef LAYERS_H
#define LAYERS_H

#define LAYERS_NUM_CONDITIONS 8 // 3 fn keys, so 8 possible combinations

#include "kbd.h"

extern uint8_t layerMatrix0[KBD_COLS][KBD_ROWS];
extern uint8_t layerMatrix1[KBD_COLS][KBD_ROWS];
extern uint8_t layerMatrix2[KBD_COLS][KBD_ROWS];
extern uint8_t layerMatrix3[KBD_COLS][KBD_ROWS];
extern uint8_t layerConditions[LAYERS_NUM_CONDITIONS];

void    layersLoad(void);
void    layersStore(void);
uint8_t layersWhichLayer(void);

static inline uint8_t (*layersMatrix(uint8_t layer))[KBD_COLS][KBD_ROWS] {
	switch (layer) {
	case 1:
		return &layerMatrix1;
	case 2:
		return &layerMatrix2;
	case 3:
		return &layerMatrix3;
	default:
		return &layerMatrix0;
	}
}

#endif
