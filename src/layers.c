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
#include "layers.h"

uint8_t layersMatrix[KBD_COLS][KBD_ROWS];
uint8_t layersConditions[LAYERS_NUM_CONDITIONS];
uint8_t layersDefaultLayer;
uint8_t layersSelectedLayer;

static void     layersLoadConditions(void);
static uint8_t *layersEEPAddr(uint8_t layer);

/*
 *
 */
void
layersInit(void)
{
	layersDefaultLayer = 0;
	layersLoad(0);
	layersLoadConditions();
}

/*
 *
 */
uint8_t *
layersEEPAddr(uint8_t layer)
{
	switch (layer) {
	case 0: return (uint8_t *)EEP_LAYER_MATRIX_0;
	case 1: return (uint8_t *)EEP_LAYER_MATRIX_1;
	case 2: return (uint8_t *)EEP_LAYER_MATRIX_2;
	case 3: return (uint8_t *)EEP_LAYER_MATRIX_3;
	}

	return (uint8_t *)EEP_LAYER_MATRIX_0;
}

/*
 *
 */
void
layersLoad(uint8_t layer)
{
	const uint8_t *addr = layersEEPAddr(layer);

	for (uint8_t col = 0; col < KBD_COLS; col++) {
		for (uint8_t row = 0; row < KBD_ROWS; row++) {
			uint8_t sc = eeprom_read_byte(addr++);
			if (sc == 0xff)
				sc = KBD_SC_IGNORED;
			layersMatrix[col][row] = sc;
		}
	}

	layersSelectedLayer = layer;
}

/*
 *
 */
void
layersSetScancode(uint8_t layer, uint8_t col, uint8_t row, uint8_t sc)
{
	uint8_t *addr = layersEEPAddr(layer);

	if (layer == layersSelectedLayer)
		layersMatrix[col][row] = sc;

	addr += (col * KBD_ROWS) + row;

	eeprom_update_byte(addr, sc);
}

/*
 *
 */
void
layersLoadConditions(void)
{
	eeprom_read_block(layersConditions,
			  (const void *)EEP_LAYER_CNDS,
			  sizeof(layersConditions));
}

/*
 *
 */
void
layersStoreConditions(void)
{
	eeprom_update_block(layersConditions,
			    (void *)EEP_LAYER_CNDS,
			    sizeof(layersConditions));
}

/*
 *
 */
void
layersSetCondition(uint8_t idx, uint8_t val)
{
	layersConditions[idx] = val;
	eeprom_update_byte((uint8_t *)EEP_LAYER_CNDS + idx, val);
}

/*
 * check for Fn or Select keys in order to switch layer
 */
void
layersProcessScan(void)
{
	/* first look for select keycodes to change default layer */
	if (kbdSCBmp[KBD_SC_SELECT_0 / 8] & (1 << (KBD_SC_SELECT_0 % 8)))
		layersDefaultLayer = 0;
	else if (kbdSCBmp[KBD_SC_SELECT_1 / 8] & (1 << (KBD_SC_SELECT_1 % 8)))
		layersDefaultLayer = 1;
	else if (kbdSCBmp[KBD_SC_SELECT_2 / 8] & (1 << (KBD_SC_SELECT_2 % 8)))
		layersDefaultLayer = 2;
	else if (kbdSCBmp[KBD_SC_SELECT_3 / 8] & (1 << (KBD_SC_SELECT_3 % 8)))
		layersDefaultLayer = 3;

	uint8_t result = layersDefaultLayer;

	/* we can rely on fn keys scancodes being byte-aligned, but we only
	 * want the lower three bits
	 */
	uint8_t fnKeys = (kbdSCBmp[KBD_SC_FN1 / 8] & 0x7);
	if (fnKeys != 0) {
		/* condition is in high nibble, resulting layer is in low */
		for (uint8_t i = 0; i < LAYERS_NUM_CONDITIONS; i++)
			if (layersConditions[i] >> 4 == fnKeys)
				result = layersConditions[i] & 0xf;
	}

	if (result != layersSelectedLayer)
		layersLoad(result);
}
