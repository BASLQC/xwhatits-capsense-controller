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
#include "layers.h"

uint8_t layerMatrix0[KBD_COLS][KBD_ROWS];
uint8_t layerMatrix1[KBD_COLS][KBD_ROWS];
uint8_t layerMatrix2[KBD_COLS][KBD_ROWS];
uint8_t layerMatrix3[KBD_COLS][KBD_ROWS];
uint8_t layerConditions[LAYERS_NUM_CONDITIONS];

static void layersLoadConditions(void);
static void layersStoreConditions(void);
static void layersLoadMatrix(const uint8_t *addr,
			     uint8_t mtx[KBD_COLS][KBD_ROWS]);
static void layersStoreMatrix(uint8_t *addr, uint8_t mtx[KBD_COLS][KBD_ROWS]);

/*
 *
 */
void
layersLoad(void)
{
	layersLoadConditions();
	layersLoadMatrix((const uint8_t *)EEP_LAYER_MATRIX_0, layerMatrix0);
	layersLoadMatrix((const uint8_t *)EEP_LAYER_MATRIX_1, layerMatrix1);
	layersLoadMatrix((const uint8_t *)EEP_LAYER_MATRIX_2, layerMatrix2);
	layersLoadMatrix((const uint8_t *)EEP_LAYER_MATRIX_3, layerMatrix3);
}

/*
 *
 */
void
layersStore(void)
{
	layersStoreConditions();
	layersStoreMatrix((uint8_t *)EEP_LAYER_MATRIX_0, layerMatrix0);
	layersStoreMatrix((uint8_t *)EEP_LAYER_MATRIX_1, layerMatrix1);
	layersStoreMatrix((uint8_t *)EEP_LAYER_MATRIX_2, layerMatrix2);
	layersStoreMatrix((uint8_t *)EEP_LAYER_MATRIX_3, layerMatrix3);
}

/*
 *
 */
void
layersLoadConditions(void)
{
	eeprom_read_block(layerConditions,
			  (const void *)EEP_LAYER_CNDS,
			  sizeof(layerConditions));
}

/*
 *
 */
void
layersStoreConditions(void)
{
	eeprom_update_block(layerConditions,
			    (void *)EEP_LAYER_CNDS,
			    sizeof(layerConditions));
}

/*
 *
 */
void
layersLoadMatrix(const uint8_t *addr, uint8_t mtx[KBD_COLS][KBD_ROWS])
{
	for (uint8_t col = 0; col < KBD_COLS; col++) {
		for (uint8_t row = 0; row < KBD_ROWS; row++) {
			uint8_t sc = eeprom_read_byte(addr++);
			if (sc == 0xff)
				sc = KBD_SC_IGNORED;
			mtx[col][row] = sc;
		}
	}
}

/*
 * Stored as one contiguous block, column-major order
 */
void
layersStoreMatrix(uint8_t *addr, uint8_t mtx[KBD_COLS][KBD_ROWS])
{
	for (uint8_t col = 0; col < KBD_COLS; col++)
		for (uint8_t row = 0; row < KBD_ROWS; row++)
			eeprom_update_byte(addr++, mtx[col][row]);
}

/*
 * Processes currently-pressed keys, if any function keys are pressed then
 * returns appropriate layer.
 */
uint8_t
layersWhichLayer(void)
{
	uint8_t fnKeys = 0;

	for (uint8_t col = 0; col < KBD_COLS; col++) {
		for (uint8_t row = 0; row < KBD_ROWS; row++) {
			if (kbdBitmap[KBD_BMP_BYTE(col)] &
			    KBD_BMP_MASK(col, row)) {
				switch (layerMatrix0[col][row]) {
				case KBD_SC_FN1:
					fnKeys |= (1 << 0);
					break;
				case KBD_SC_FN2:
					fnKeys |= (1 << 1);
					break;
				case KBD_SC_FN3:
					fnKeys |= (1 << 2);
					break;
				}
			}
		}
	}

	if (fnKeys == 0)
		return 0;

	/* condition is in high nibble, resulting layer is in low */
	for (uint8_t i = 0; i < LAYERS_NUM_CONDITIONS; i++)
		if (layerConditions[i] >> 4 == fnKeys)
			return layerConditions[i] & 0xf;

	return 0;
}
