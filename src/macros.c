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
#include "macros.h"

/*
 * EEPROM format looks like this:
 * (1 byte scancode)
 * (1 byte modStates)
 * (1 byte modMask)
 * (1 byte numMakeBytes)
 * (1 byte numBreakBytes)
 * ...then repeated:
 * (1 byte cmd)
 * (1 byte val) <-- optional
 *
 * If it's the last macro, it's followed by a null byte.
 */
#define MACROS_OFFS_MAKEBYTES  3
#define MACROS_OFFS_BREAKBYTES (MACROS_OFFS_MAKEBYTES + 1)
#define MACROS_OFFS_FIRSTCMD   (MACROS_OFFS_BREAKBYTES + 1)

typedef enum {
	msIdle
} MacroState;

static MacroDef macrosDefs[MACROS_NUM_MACROS];

static uint8_t modStack[8];
static int8_t  modStackIdx;

static uint8_t macroMods;
static uint8_t prevKbdMods;

static int8_t currMacro;
static int8_t currPos;
static bool   haveAdvanced;
static int8_t remainingMakeBytes;
static int8_t remainingBreakBytes;
static bool   retainPressedMods;

static uint8_t dlyRemainingTenMS;
static uint8_t dlyMSTicks;

/*
 *
 */
void
macrosInit(void)
{
	modStackIdx = -1;
	currMacro   = -1;
	macroMods   =  0;
	macrosLoad();
}

/*
 * returns pointer to start of next macro in EEPROM
 */
static uint8_t *
macrosLoadDef(uint8_t macroIdx, uint8_t *addr)
{
	memset(&macrosDefs[macroIdx], 0, sizeof(MacroDef));

	/* check there's room for a header and at least one command */
	if (addr + 6 >= (uint8_t *)EEP_MACROS_BND)
		return (uint8_t *)EEP_MACROS_BND;

	/* read header */
	uint8_t sc = eeprom_read_byte(addr);
	if (sc == 0x00 || sc == 0xff)
		return (uint8_t *)EEP_MACROS_BND;
	macrosDefs[macroIdx].addr      = addr++;
	macrosDefs[macroIdx].scancode  = sc;
	macrosDefs[macroIdx].modStates = eeprom_read_byte(addr++);
	macrosDefs[macroIdx].modMask   = eeprom_read_byte(addr++);

	/* find start of next header */
	addr += ((eeprom_read_byte(addr)) +
		 (eeprom_read_byte(addr + 1)));
	addr += 2;

	return addr;
}

/*
 *
 */
void
macrosLoad(void)
{
	uint8_t *addr = (uint8_t *)EEP_MACROS;
	for (uint8_t i = 0; i < MACROS_NUM_MACROS; i++)
		addr = macrosLoadDef(i, addr);
}

/*
 *
 */
static void
incPos(uint8_t bytes)
{
	if (remainingMakeBytes > 0) {
		currPos += bytes;
		remainingMakeBytes -= bytes;
		if (remainingMakeBytes < 0)
			remainingMakeBytes = 0;
	} else if (remainingBreakBytes > 0) {
		currPos += bytes;
		remainingBreakBytes -= bytes;
		if (remainingBreakBytes < 0)
			remainingBreakBytes = 0;
	}

	haveAdvanced = true;
}

/*
 *
 */
static void
macroPress(uint8_t sc)
{
	static bool haveMade = false;
	if (!haveMade) {
		haveMade = true;
		if (sc <= HID_KEYBOARD_SC_RIGHT_GUI)
			kbdSCBmp[sc / 8] |=  (1 << (sc % 8));
	} else {
		haveMade = false;
		if (sc <= HID_KEYBOARD_SC_RIGHT_GUI)
			kbdSCBmp[sc / 8] &= ~(1 << (sc % 8));
		incPos(2);
	}
}

/*
 *
 */
static void
macroAssignMods(uint8_t mods)
{
	incPos(2);
	macroMods = mods;
}

/*
 *
 */
static void
macroSetMods(uint8_t mods)
{
	incPos(2);
	macroMods |= mods;
}

/*
 *
 */
static void
macroClearMods(uint8_t mods)
{
	incPos(2);
	macroMods &= ~mods;
}

/*
 *
 */
static void
macroToggleMods(uint8_t mods)
{
	incPos(2);
	macroMods ^= mods;
}

/*
 * we don't incPos here, as it's used in conjunction with other commands
 */
static void
macroPushMods(void)
{
	/* don't overflow stack */
	if (modStackIdx >= (int8_t)(sizeof(modStack) - 1)) {
		modStackIdx = sizeof(modStack) - 1;
		return;
	}

	modStackIdx++;
	modStack[modStackIdx] = macroMods;
}

/*
 *
 */
static void
macroPopMods(void)
{
	incPos(1);

	/* don't underflow stack */
	if (modStackIdx < 0) {
		modStackIdx = -1;
		return;
	}

	macroMods = modStack[modStackIdx];
	modStackIdx--;
}

/*
 *
 */
static void
macroPopAllMods(void)
{
	incPos(1);

	/* don't do anything if the stack is empty */
	if (modStackIdx < 0) {
		modStackIdx = -1;
		return;
	}

	macroMods = modStack[0];
	modStackIdx = -1;

}

/*
 *
 */
static void
macroDelay(uint8_t val)
{
	static bool haveStarted = false;

	if (!haveStarted) {
		haveStarted = true;
		dlyRemainingTenMS = val;
	} else if (dlyRemainingTenMS == 0) {
		haveStarted = false;
		incPos(2);
	}
}

/*
 *
 */
static void
clearTriggerSC(void)
{
	uint8_t sc = macrosDefs[currMacro].scancode;
	kbdSCBmp[sc / 8] &= ~(1 << (sc % 8));
}

/*
 * While playing macros, we want to retain control over mods, but if new mods
 * are pressed or released while it's playing then include/remove them. We
 * can't use the kbdSCBmp/kbdPrevSCBmp as a method, as that's being modified by
 * our macro.
 */
static void
captureModChanges(void)
{
	const uint8_t currMods = kbdSCMods(kbdSCBmp);

	uint8_t newlyPressedMods  = ( currMods & ~prevKbdMods);
	uint8_t newlyReleasedMods = (~currMods & prevKbdMods);

	macroMods |=  newlyPressedMods;
	macroMods &= ~newlyReleasedMods;

	prevKbdMods = currMods;
}

/*
 *
 */
static bool
cmdHasVal(uint8_t cmd)
{
	switch (cmd)
	{
	case mcPress:
	case mcAssignMods:
	case mcSetMods:
	case mcClearMods:
	case mcToggleMods:
	case mcDelay:
		return true;
	default:
		return false;
	}
}

/*
 * returns true if we changed kbdSCBmp
 */
static bool
playCurrMacro(void)
{
	uint8_t *addr = macrosDefs[currMacro].addr;

	captureModChanges();

	if (currPos == -1) {
		remainingMakeBytes = eeprom_read_byte(addr +
						      MACROS_OFFS_MAKEBYTES);
		remainingBreakBytes = -1;
		currPos = 0;
		clearTriggerSC();
		return false;
	} else if (remainingMakeBytes == 0 && remainingBreakBytes == -1) {
		if (kbdSCIsIn(macrosDefs[currMacro].scancode, kbdSCBmp)) {
			clearTriggerSC();
			return false;
		}

		remainingBreakBytes = eeprom_read_byte(addr +
						       MACROS_OFFS_BREAKBYTES);
		if (remainingBreakBytes & (1 << 7)) {
			retainPressedMods = true;
			remainingBreakBytes &= ~(1 << 7);
		} else
			retainPressedMods = false;
		return false;
	} else if (remainingMakeBytes == 0 && remainingBreakBytes == 0) {
		currMacro = -1;
		clearTriggerSC();
		if (!retainPressedMods)
			macroMods = 0;
		return false;
	}

	clearTriggerSC();

	addr += (MACROS_OFFS_FIRSTCMD + currPos);
	uint8_t cmd = eeprom_read_byte(addr);

	bool pushMods;
	if (cmd & mcPushMods) {
		pushMods = true;
		cmd &= ~mcPushMods;
	} else
		pushMods = false;

	uint8_t val;
	if (cmdHasVal(cmd))
		val = eeprom_read_byte(addr + 1);

	/* don't push twice or more if haven't advanced from previous command */
	if (haveAdvanced && pushMods)
		macroPushMods();

	haveAdvanced = false;
	bool scBmpChanged = true;
	switch (cmd) {
	case mcPress:
		macroPress(val);
		break;
	case mcAssignMods:
		macroAssignMods(val);
		break;
	case mcSetMods:
		macroSetMods(val);
		break;
	case mcClearMods:
		macroClearMods(val);
		break;
	case mcToggleMods:
		macroToggleMods(val);
		break;
	case mcPopMods:
		macroPopMods();
		break;
	case mcPopAllMods:
		macroPopAllMods();
		break;
	case mcDelay:
		macroDelay(val);
		scBmpChanged = false;
		break;
	default:
		incPos(1);
		scBmpChanged = false;
		break;
	}

	/* assign our version of the mods */
	kbdSCMods(kbdSCBmp) = macroMods;

	return scBmpChanged;
}

/*
 *
 */
static void
startMacro(uint8_t idx)
{
	currMacro = idx;
	currPos   = -1;

	dlyRemainingTenMS = 0;
	dlyMSTicks        = 0;

	haveAdvanced = true;

	clearTriggerSC();
	macroMods = prevKbdMods = kbdSCMods(kbdSCBmp);
}

/*
 *
 */
bool
macrosProcessScan(void)
{
	if (currMacro != -1)
		return playCurrMacro();

	/* if macro is not playing, apply saved mods (if any) */
	kbdSCMods(kbdSCBmp) = (kbdSCMods(kbdSCBmp) | macroMods);

	for (uint8_t i = 0; i < MACROS_NUM_MACROS; i++) {
		uint8_t sc = macrosDefs[i].scancode;
		if (sc == KBD_SC_IGNORED)
			break;

		/* match scancode key-down change-of-state */
		if (!( kbdSCIsIn(sc, kbdSCBmp) &&
		      !kbdSCIsIn(sc, kbdPrevSCBmp)))
			continue;

		uint8_t state = macrosDefs[i].modStates;
		uint8_t mask  = macrosDefs[i].modMask;
		uint8_t mods  = kbdSCMods(kbdSCBmp);

		/* reject if a mod is present that has been excluded */
		if (mods & (~state & mask))
			continue;

		if ((state & mask) == (mods & mask)) {
			/* have basic match */
			startMacro(i);
			break;
		} else if (!(mods & mask)) {
			/* check for special "either-or" match */
			uint8_t orState = (state & state >> 4) & mask;
			uint8_t orMods  = (mods  | mods  >> 4) & mask;
			if (orMods && (orState == orMods)) {
				startMacro(i);
				break;
			}
		}
	}

	return false;
}

/*
 * stored as tens of milliseconds
 */
void
macrosMSTick(void)
{
	if (dlyRemainingTenMS == 0)
		return;

	dlyMSTicks++;
	if (dlyMSTicks == 10) {
		dlyMSTicks = 0;
		dlyRemainingTenMS--;
	}
}
