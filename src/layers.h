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
#ifndef LAYERS_H
#define LAYERS_H

#define LAYERS_NUM_CONDITIONS 8 // 3 fn keys, so 8 possible combinations

#include "kbd.h"

extern uint8_t layersMatrix[KBD_COLS][KBD_ROWS];
extern uint8_t layersConditions[LAYERS_NUM_CONDITIONS];
extern uint8_t layersDefaultLayer;
extern uint8_t layersSelectedLayer;

void layersInit(void);
void layersLoad(uint8_t layer);
void layersSetScancode(uint8_t layer, uint8_t col, uint8_t row, uint8_t sc);
void layersSetCondition(uint8_t idx, uint8_t val);
void layersProcessScan(void);

#endif
