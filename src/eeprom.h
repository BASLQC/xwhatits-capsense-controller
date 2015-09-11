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
#ifndef EEPROM_H
#define EEPROM_H

#include <avr/eeprom.h>

/* offsets for eeprom locations */

#define EEP_BASE                0

#define EEP_VREF_L              (EEP_BASE)
#define EEP_VREF_H              (EEP_VREF_L + 1)

#define EEP_LAYER_MATRIX_0      (EEP_VREF_H + 1)
#define EEP_LAYER_MATRIX_0_BND  (EEP_LAYER_MATRIX_0 + (KBD_COLS * KBD_ROWS))

#define EEP_LAYER_MATRIX_1      (EEP_LAYER_MATRIX_0_BND)
#define EEP_LAYER_MATRIX_1_BND  (EEP_LAYER_MATRIX_1 + (KBD_COLS * KBD_ROWS))

#define EEP_LAYER_MATRIX_2      (EEP_LAYER_MATRIX_1_BND)
#define EEP_LAYER_MATRIX_2_BND  (EEP_LAYER_MATRIX_2 + (KBD_COLS * KBD_ROWS))

#define EEP_LAYER_MATRIX_3      (EEP_LAYER_MATRIX_2_BND)
#define EEP_LAYER_MATRIX_3_BND  (EEP_LAYER_MATRIX_3 + (KBD_COLS * KBD_ROWS))

#define EEP_LAYER_CNDS          (EEP_LAYER_MATRIX_3_BND)
#define EEP_LAYER_CNDS_BND      (EEP_LAYER_CNDS + LAYERS_NUM_CONDITIONS)

#define EEP_EXP_MODE            (EEP_LAYER_CNDS_BND)
#define EEP_EXP_VAL1            (EEP_EXP_MODE + 1)
#define EEP_EXP_VAL2            (EEP_EXP_VAL1 + 1)

#define EEP_KBD_COL_SKIPS	(EEP_EXP_VAL2 + 1)
#define EEP_KBD_COL_SKIPS_BND	(EEP_KBD_COL_SKIPS + sizeof(kbdColSkips))

#endif
