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
#include "kbd.h"

uint8_t kbdSCBmp[29];   // 29 bytes allows up to 0xe7 (Right GUI)
uint8_t kbdPrevSCBmp[sizeof(kbdSCBmp)];
uint8_t kbdLEDs;
uint8_t kbdColSkips[3]; // enough for 24 columns, waste one byte with 16

/*
 *
 */
void
kbdInit(void)
{
	kbdLoadColSkips();
}

/*
 *
 */
void
kbdCalibrate(void)
{
	uint16_t rMin, rMax, val = 0;
	uint16_t loPoint, hiPoint;
	uint8_t oldSelectedLayer = layersSelectedLayer;

	uint8_t numLO = 0;
	uint8_t numHI = 0;

	layersLoad(0);

	/* count totals for HI/LO keys considered for calibration; we include
	 * standard scancodes (i.e. not just KBD_SC_CAL_LO keys) in the LO keys
	 * as it gives a better picture (so long as you aren't holding any keys
	 * while you plug the keyboard in)
	 */
	for (uint8_t col = 0; col < KBD_COLS; col++) {
		if (kbdColSkips[col / 8] & (1 << (col % 8)))
			continue;

		for (uint8_t row = 0; row < KBD_ROWS; row++) {
			switch (layersMatrix[col][row]) {
			case KBD_SC_IGNORED:
				break;
			case KBD_SC_CAL_HI:
				numHI++;
				break;
			default:
				numLO++;
				break;
			}
		}
	}

	/* first look for point where all LO keys first start staying LO; will
	 * be the higher number for active-low keyboards and the lower number
	 * for active-high keyboards
	 */
	rMin = 0;
	rMax = VREF_MAX;
	while (rMax - rMin > 1) {
		val = ((rMax - rMin) / 2) + rMin;
		vrefSet(val);

		/* wait for a few scans */
		scanTick = 0;
		while (scanTick < 20)
			;

		uint8_t releasedLoKeys = 0;
		for (uint8_t col = 0; col < KBD_COLS; col++) {
			if (kbdColSkips[col / 8] & (1 << (col % 8)))
				continue;

			for (uint8_t row = 0; row < KBD_ROWS; row++) {
				if (scanState[col][row] < SCAN_DB_THRESH) {
					uint8_t sc = layersMatrix[col][row];
					if (sc != KBD_SC_IGNORED &&
					    sc != KBD_SC_CAL_HI)
						releasedLoKeys++;
				}
			}
		}

#if defined(KBD_ACTIVE_LOW)
		if (releasedLoKeys < numLO)
			rMax = val;
		else
			rMin = val;
#elif defined(KBD_ACTIVE_HIGH)
		if (releasedLoKeys < numLO)
			rMin = val;
		else
			rMax = val;
#endif
	}
#if defined(KBD_ACTIVE_LOW)
	hiPoint = val;
#elif defined(KBD_ACTIVE_HIGH)
	loPoint = val;
#endif

	/* now look for point where all HI keys first start staying HI; will
	 * be the lower number for active-low keyboards and the higher number
	 * for active-high keyboards
	 */
	rMin = 0;
	rMax = VREF_MAX;
	while (rMax - rMin > 1) {
		val = ((rMax - rMin) / 2) + rMin;
		vrefSet(val);

		/* wait for a few scans */
		scanTick = 0;
		while (scanTick < 20)
			;

		uint8_t pressedHiKeys = 0;
		for (uint8_t col = 0; col < KBD_COLS; col++) {
			if (kbdColSkips[col / 8] & (1 << (col % 8)))
				continue;

			for (uint8_t row = 0; row < KBD_ROWS; row++) {
				if (scanState[col][row] >= SCAN_DB_THRESH) {
					uint8_t sc = layersMatrix[col][row];
					if (sc == KBD_SC_CAL_HI)
						pressedHiKeys++;
				}
			}
		}

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
#if defined(KBD_ACTIVE_LOW)
	loPoint = val;
#elif defined(KBD_ACTIVE_HIGH)
	hiPoint = val;
#endif

	/* Set resulting value part-way between. Your random fraction for today
	 * is one third.
	 */
#if defined(KBD_ACTIVE_LOW)
	vrefSet(((hiPoint - loPoint) / 3) * 1 + loPoint);
#elif defined(KBD_ACTIVE_HIGH)
	vrefSet(((hiPoint - loPoint) / 3) * 2 + loPoint);
#endif

	expReset();
	layersLoad(oldSelectedLayer);
}

/*
 * returns true if anything changed
 */
bool
kbdUpdateSCBmp(void)
{
	for (uint8_t i = 0; i < sizeof(kbdSCBmp); i++) {
		kbdPrevSCBmp[i] = kbdSCBmp[i];
		kbdSCBmp[i] = 0;
	}

	for (uint8_t row = 0; row < KBD_ROWS; row++) {
		for (uint8_t col = 0; col < KBD_COLS; col++) {
			if (scanState[col][row] >= SCAN_DB_THRESH) {
				uint8_t sc = layersMatrix[col][row];
				if (sc <= 0xe7)
					kbdSCBmp[sc / 8] |= (1 << (sc % 8));
			}
		}
	}

	for (uint8_t i = 0; i < sizeof(kbdSCBmp); i++)
		if (kbdPrevSCBmp[i] != kbdSCBmp[i])
			return true;
	return false;
}

/*
 *
 */
void
kbdFillReport(USB_KeyboardReport_Data_t *kbdReport)
{
	uint8_t usedKeyCodes = 0;

	/* check for standard keys, between 0x04 (a) and 0xa4 (ExSel) */
	for (uint8_t i = (HID_KEYBOARD_SC_A / 8);
	     i <= (HID_KEYBOARD_SC_EXSEL / 8);
	     i++) {
		/* check if any keys pressed within this byte */
		if (!kbdSCBmp[i])
			continue;

		/* look at each bit for a pressed key */
		for (uint8_t j = 0; j < 8; j++) {
			if (kbdSCBmp[i] & (1 << j)) {
				uint8_t sc = i * 8 + j;
				if (sc >= HID_KEYBOARD_SC_A &&
				    sc <= HID_KEYBOARD_SC_EXSEL)
					kbdReport->KeyCode[usedKeyCodes++] = sc;
			}
		}
	}

	/* copy modifiers straight across; 0xe0--0xe7 are whole, in byte 28 */
	kbdReport->Modifier = kbdSCBmp[HID_KEYBOARD_SC_LEFT_CONTROL / 8];
}

/*
 *
 */
void
kbdFillNKROReport(NKROReport *report)
{
	/* Copy standard keys---between 0x04 (a) and 0xa4 (ExSel)---straight
	 * across, as NKRO report has same bitmap for the lower parts.
	 */
	for (uint8_t i = (HID_KEYBOARD_SC_A / 8);
	     i <= (HID_KEYBOARD_SC_EXSEL / 8);
	     i++)
		report->codeBmp[i] = kbdSCBmp[i];

	/* zero scancodes at the beginning and the end that don't belong in this
	 * report: 0x00--0x02 (ignored, pressed, released) and 0xa5--0xa7
	 * (system power, sleep, wake)
	 */
	report->codeBmp[0]  &= 0xf8;
	report->codeBmp[20] &= 0x1f;


	/* copy modifiers straight across; 0xe0--0xe7 are whole, in byte 28 */
	report->modifiers = kbdSCBmp[HID_KEYBOARD_SC_LEFT_CONTROL / 8];
}

/*
 * because report is pretty small, keep a copy of the previous one sent and
 * compare, returning true if different
 */
bool
kbdFillSystemReport(ExtrakeySystemReport *report)
{
	static ExtrakeySystemReport prevReport;

	/* Copy the three scancodes we're concerned with---0xa5 (system power),
	 * 0xa6 (system sleep) and 0xa7 (system wake).
	 */
	report->codeBmp = (kbdSCBmp[KBD_SC_SYSTEM_POWER / 8] >> 5);

	if (prevReport.codeBmp != report->codeBmp) {
		prevReport.codeBmp = report->codeBmp;
		return true;
	}

	return false;
}

/*
 * see kbdFillSystemReport comment re. return
 */
bool
kbdFillConsumerReport(ExtrakeyConsumerReport *report)
{
	static ExtrakeyConsumerReport prevReport;

	/* Copy the three bytes for the consumer keys into the report;
	 * scancodes 0xa8 (media play) through 0xbd (web bookmarks).  0xa8 is
	 * byte-aligned so just do straight copy. Zero the two padding bits at
	 * the end; they shouldn't be set but who knows if there's a duff
	 * scancode floating around.
	 */
	report->codeBmp[0] =  kbdSCBmp[(KBD_SC_MEDIA_PLAY / 8)];
	report->codeBmp[1] =  kbdSCBmp[(KBD_SC_MEDIA_PLAY / 8) + 1];
	report->codeBmp[2] = (kbdSCBmp[(KBD_SC_MEDIA_PLAY / 8) + 2] & 0x3f);

	bool changed = false;
	for (uint8_t i = 0; i < sizeof(report->codeBmp); i++) {
		if (prevReport.codeBmp[i] != report->codeBmp[i]) {
			changed = true;
			prevReport.codeBmp[i] = report->codeBmp[i];
		}
	}

	return changed;
}

/*
 *
 */
void
kbdReceiveReport(const void *report)
{
	kbdLEDs = ((uint8_t *)report)[0];
}

/*
 *
 */
void
kbdReceiveNKROReport(const void *report)
{
	kbdLEDs = ((uint8_t *)report)[0];
}

/*
 *
 */
bool
kbdWantsWakeup(void)
{
	for (uint8_t i = (HID_KEYBOARD_SC_A / 8);
	     i <= (HID_KEYBOARD_SC_RIGHT_GUI / 8);
	     i++) {
		if (!kbdSCBmp[i])
			continue;

		for (uint8_t j = 0; j < 8; j++) {
			if (kbdSCBmp[i] & (1 << j)) {
				uint8_t sc = i * 8 + j;
				if (sc >= HID_KEYBOARD_SC_A)
					return true;
			}
		}
	}

	return false;
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
