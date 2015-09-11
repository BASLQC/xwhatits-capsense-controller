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
#include "kbd.h"

int8_t	kbd[KBD_COLS][KBD_ROWS];
uint8_t	kbdBitmap[(KBD_ROWS * KBD_COLS + 8 - 1) / 8];
uint8_t	kbdColSkips[3]; // enough for 24 columns, waste one byte with 16

static void kbdScanColumn(uint8_t col);

/*
 *
 */
void
kbdInit(void)
{
	memset(kbd, 0, sizeof(kbd[0][0]) * KBD_COLS * KBD_ROWS);
	memset(kbdBitmap, 0, sizeof(kbdBitmap));
	layersDefaultLayer = 0;
}

/*
 *
 */
void
kbdCalibrate(void)
{
	uint16_t rMin, rMax, val = 0;
	uint16_t loPoint, hiPoint;

	uint8_t numLO = 0;
	uint8_t numHI = 0;

	for (uint8_t col = 0; col < KBD_COLS; col++) {
		for (uint8_t row = 0; row < KBD_ROWS; row++) {
			switch (layerMatrix0[col][row]) {
			case KBD_SC_CAL_HI:
				numHI++;
				break;
			case KBD_SC_CAL_LO:
				numLO++;
				break;
			}
		}
	}

	/* first look point where all LO keys first start staying LO */
	rMin = 0;
	rMax = VREF_MAX;
	while (rMax - rMin > 1) {
		val = ((rMax - rMin) / 2) + rMin;
		vrefSet(val);
		_delay_us(100); // wait for DAC to settle

		for (uint8_t i = 0; i < 32; i++)
			kbdScan();

		uint8_t pressedLoKeys = 0;
		for (uint8_t col = 0; col < KBD_COLS; col++)
			for (uint8_t row = 0; row < KBD_ROWS; row++)
				if (kbd[col][row] > KBD_DEBOUNCE_THRESH &&
				    layerMatrix0[col][row] >= HID_KEYBOARD_SC_A)
					pressedLoKeys++;

#if defined(KBD_ACTIVE_LOW)
		if (pressedLoKeys < numLO)
			rMin = val;
		else
			rMax = val;
#elif defined(KBD_ACTIVE_HIGH)
		if (pressedLoKeys < numLO)
			rMax = val;
		else
			rMin = val;
#endif
	}
	loPoint = val;

	/* now look for point where all HI keys first start staying LO */
	rMin = 0;
	rMax = VREF_MAX;
	while (rMax - rMin > 1) {
		val = ((rMax - rMin) / 2) + rMin;
		vrefSet(val);
		_delay_us(100); // wait for DAC to settle

		for (uint8_t i = 0; i < 32; i++)
			kbdScan();

		uint8_t pressedHiKeys = 0;
		for (uint8_t col = 0; col < KBD_COLS; col++)
			for (uint8_t row = 0; row < KBD_ROWS; row++)
				if (kbd[col][row] > KBD_DEBOUNCE_THRESH &&
				    layerMatrix0[col][row] == KBD_SC_CAL_HI)
					pressedHiKeys++;

#if defined(KBD_ACTIVE_LOW)
		if (pressedHiKeys < numHI)
			rMin = val;
		else
			rMax = val;
#elif defined(KBD_ACTIVE_HIGH)
		if (pressedHiKeys < numHI)
			rMax = val;
		else
			rMin = val;
#endif
	}
	hiPoint = val;

	/* Set resulting value part-way between. Your random fraction for today
	 * is seven eighths.
	 */
#if defined(KBD_ACTIVE_LOW)
	vrefSet(((loPoint - hiPoint) / 8) * 7 + hiPoint);
#elif defined(KBD_ACTIVE_HIGH)
	vrefSet(((hiPoint - loPoint) / 8) * 7 + loPoint);
#endif

	expReset();
}

/*
 *
 */
void
kbdFillReport(USB_KeyboardReport_Data_t *kbdReport)
{
	uint8_t usedKeyCodes = 0;

	uint8_t (*mtx)[KBD_COLS][KBD_ROWS] = layersMatrix(layersWhichLayer());

	for (uint8_t col = 0; col < KBD_COLS; col++) {
		for (uint8_t row = 0;
		     row < KBD_ROWS && usedKeyCodes < 6;
		     row++) {
			if (kbdBitmap[KBD_BMP_BYTE(col)] &
			    KBD_BMP_MASK(col, row)) {
				uint8_t sc = (*mtx)[col][row];

				if (sc < HID_KEYBOARD_SC_A ||
				    (sc >= KBD_SC_FN1 && sc <= KBD_SC_FN3))
					continue;

				kbdReport->KeyCode[usedKeyCodes++] = sc;
				switch (sc) {
				case KBD_SC_SELECT_0:
					layersDefaultLayer = 0;
					break;
				case KBD_SC_SELECT_1:
					layersDefaultLayer = 1;
					break;
				case KBD_SC_SELECT_2:
					layersDefaultLayer = 2;
					break;
				case KBD_SC_SELECT_3:
					layersDefaultLayer = 3;
					break;
				case HID_KEYBOARD_SC_LEFT_SHIFT:
					kbdReport->Modifier +=
					   HID_KEYBOARD_MODIFIER_LEFTSHIFT;
					break;
				case HID_KEYBOARD_SC_RIGHT_SHIFT:
					kbdReport->Modifier +=
					   HID_KEYBOARD_MODIFIER_RIGHTSHIFT;
					break;
				case HID_KEYBOARD_SC_LEFT_CONTROL:
					kbdReport->Modifier +=
					   HID_KEYBOARD_MODIFIER_LEFTCTRL;
					break;
				case HID_KEYBOARD_SC_RIGHT_CONTROL:
					kbdReport->Modifier +=
					   HID_KEYBOARD_MODIFIER_RIGHTCTRL;
					break;
				case HID_KEYBOARD_SC_LEFT_ALT:
					kbdReport->Modifier +=
					   HID_KEYBOARD_MODIFIER_LEFTALT;
					break;
				case HID_KEYBOARD_SC_RIGHT_ALT:
					kbdReport->Modifier +=
					   HID_KEYBOARD_MODIFIER_RIGHTALT;
					break;
				case HID_KEYBOARD_SC_LEFT_GUI:
					kbdReport->Modifier +=
					   HID_KEYBOARD_MODIFIER_LEFTGUI;
					break;
				case HID_KEYBOARD_SC_RIGHT_GUI:
					kbdReport->Modifier +=
					   HID_KEYBOARD_MODIFIER_RIGHTGUI;
					break;
				}
			}
		}
	}

	expPostProcessStdKbdReport(kbdReport, usedKeyCodes);
}

/*
 *
 */
void
kbdFillNKROReport(NKROReport *report, NKROReport *prevReport)
{
	uint8_t (*mtx)[KBD_COLS][KBD_ROWS] = layersMatrix(layersWhichLayer());

	for (uint8_t col = 0; col < KBD_COLS; col++) {
		for (uint8_t row = 0; row < KBD_ROWS; row++) {
			if (kbdBitmap[KBD_BMP_BYTE(col)] &
			    KBD_BMP_MASK(col, row)) {
				uint8_t sc = (*mtx)[col][row];

				switch (sc) {
				case KBD_SC_SELECT_0:
					layersDefaultLayer = 0;
					break;
				case KBD_SC_SELECT_1:
					layersDefaultLayer = 1;
					break;
				case KBD_SC_SELECT_2:
					layersDefaultLayer = 2;
					break;
				case KBD_SC_SELECT_3:
					layersDefaultLayer = 3;
					break;
				case HID_KEYBOARD_SC_LEFT_SHIFT:
				case HID_KEYBOARD_SC_RIGHT_SHIFT:
					if (!(prevReport->modifiers &
					      (1 << (sc - 0xe0))))
						expKeyPositiveEdge();
				case HID_KEYBOARD_SC_LEFT_CONTROL:
				case HID_KEYBOARD_SC_LEFT_ALT:
				case HID_KEYBOARD_SC_LEFT_GUI:
				case HID_KEYBOARD_SC_RIGHT_CONTROL:
				case HID_KEYBOARD_SC_RIGHT_ALT:
				case HID_KEYBOARD_SC_RIGHT_GUI:
					report->modifiers |= (1 << (sc - 0xe0));
					break;
				case KBD_SC_IGNORED:
				case KBD_SC_CAL_HI:
				case KBD_SC_CAL_LO:
				case KBD_SC_FN1:
				case KBD_SC_FN2:
				case KBD_SC_FN3:
					break;
				default:
					if (!(prevReport->codeBmp[sc / 8] &
					      (1 << (sc % 8))))
						expKeyPositiveEdge();
					report->codeBmp[sc / 8] |=
					   (1 << (sc % 8));
					break;
				}

			}
		}
	}

	expPostProcessNKROKbdReport(report);
}

/*
 *
 */
void
kbdReceiveReport(const void *report)
{
	const uint8_t *ledReport = report;

	expSetLockLEDs(ledReport[0]);
}

/*
 *
 */
void
kbdReceiveNKROReport(const void *report)
{
	const uint8_t *ledReport = report;

	expSetLockLEDs(ledReport[0]);
}

/*
 *
 */
void
kbdScan(void)
{
	memset(kbdBitmap, 0x00, sizeof(kbdBitmap));

	for (uint8_t i = 0; i < KBD_COLS; i++) {
		/* skip column if necessary */
		if (kbdColSkips[i / 8] & (1 << (i % 8)))
			continue;

		/* select column */
		srDisable();
		srSet(i);
		srEnable();

		/* scan */
		kbdScanColumn(i);

		/* deselect column */
		srDisable();
		srClear();
		srEnable();

		/* wait for negative spike */
		if (i < KBD_COLS - 1)
			_delay_loop_1(200);
	}
}

/*
 *
 */
void
kbdScanColumn(uint8_t col)
{
	uint8_t scan[KBD_ROWS];
	memset(scan, 0x00, KBD_ROWS);

	_delay_loop_1(1); // shift-register & LM339 propagation delay

	if (KBD_ROW_TEST(KBD_ROW1_PIN, KBD_ROW1_IP)) // 1st row
		scan[0] = 1;
	if (KBD_ROW_TEST(KBD_ROW2_PIN, KBD_ROW2_IP)) // 2nd row
		scan[1] = 1;
	if (KBD_ROW_TEST(KBD_ROW3_PIN, KBD_ROW3_IP)) // 3rd row
		scan[2] = 1;
	if (KBD_ROW_TEST(KBD_ROW4_PIN, KBD_ROW4_IP)) // 4th row
		scan[3] = 1;
#if KBD_ROWS == 8
	if (KBD_ROW_TEST(KBD_ROW5_PIN, KBD_ROW5_IP)) // 5th row
		scan[4] = 1;
	if (KBD_ROW_TEST(KBD_ROW6_PIN, KBD_ROW6_IP)) // 6th row
		scan[5] = 1;
	if (KBD_ROW_TEST(KBD_ROW7_PIN, KBD_ROW7_IP)) // 7th row
		scan[6] = 1;
	if (KBD_ROW_TEST(KBD_ROW8_PIN, KBD_ROW8_IP)) // 8th row
		scan[7] = 1;
#endif

	for (uint8_t i = 0; i < KBD_ROWS; i++) {
		if (scan[i])
			kbd[col][i]--;
		else
			kbd[col][i]++;

		if (kbd[col][i] < KBD_DEBOUNCE_MIN)
			kbd[col][i] = KBD_DEBOUNCE_MIN;
		else if (kbd[col][i] > KBD_DEBOUNCE_MAX)
			kbd[col][i] = KBD_DEBOUNCE_MAX;

		if (kbd[col][i] > KBD_DEBOUNCE_THRESH)
			kbdBitmap[KBD_BMP_BYTE(col)] |= KBD_BMP_MASK(col, i);
	}
}

/*
 *
 */
uint8_t
kbdWantsWakeup(void)
{
	for (uint8_t col = 0; col < KBD_COLS; col++)
		for (uint8_t row = 0; row < KBD_ROWS; row++)
			if (kbd[col][row] > KBD_DEBOUNCE_THRESH &&
			    layerMatrix0[col][row] >= HID_KEYBOARD_SC_A)
				return 1;
	return 0;
}

/*
 *
 */
void
kbdStoreColSkips(void)
{
	eeprom_update_block(kbdColSkips,
			    (uint8_t *)EEP_KBD_COL_SKIPS,
			    sizeof(kbdColSkips));
}

/*
 *
 */
void
kbdLoadColSkips(void)
{
	eeprom_read_block(kbdColSkips,
			  (uint8_t *)EEP_KBD_COL_SKIPS,
			  sizeof(kbdColSkips));

	/* check if everything uninitialised, in which case clear it */
	for (uint8_t i = 0; i < sizeof(kbdColSkips); i++)
		if (kbdColSkips[i] != 0xff)
			return;
	for (uint8_t i = 0; i < sizeof(kbdColSkips); i++)
		kbdColSkips[i] = 0x00;
}
