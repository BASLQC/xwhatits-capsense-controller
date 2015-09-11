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
#include "exp.h"

static uint8_t msCtr;
static uint8_t solenoidQueue;

uint8_t expMode;
uint8_t expVal1; // meaning of val 1 & 2 is depending on mode
uint8_t expVal2;

/*
 *
 */
void
expClear(void)
{
	DDRB  &= ~(1 << PB4);
	DDRB  &= ~(1 << PB5);
	DDRB  &= ~(1 << PB6);
	DDRB  &= ~(1 << PB7);
	PORTB &= ~(1 << PB4);
	PORTB &= ~(1 << PB5);
	PORTB &= ~(1 << PB6);
	PORTB &= ~(1 << PB7);

	msCtr = 0;
	solenoidQueue = 0;

	/* LEDs on the PC-AT seem to have pull-ups; so re-enable outputs so
	 * that the LEDs don't stay on and waste power when this function is
	 * called before USB suspend.
	 */
	if (expMode == expModeLockLEDs) {
		DDRB  |= (1 << PB4);
		DDRB  |= (1 << PB5);
		DDRB  |= (1 << PB6);
	}
}

/*
 *
 */
void
expReset(void)
{
	expClear();

	switch (expMode) {
	case expModeSolenoidPlusNOCapsLockSwitch:
	case expModeSolenoidPlusNCCapsLockSwitch:
	case expModeSolenoidPlusNONumLockSwitch:
	case expModeSolenoidPlusNCNumLockSwitch:
	case expModeSolenoidPlusNOShiftLockSwitch:
	case expModeSolenoidPlusNCShiftLockSwitch:
	case expModeSolenoidPlusNOFn1LockSwitch:
	case expModeSolenoidPlusNCFn1LockSwitch:
	case expModeSolenoidPlusNOFn2LockSwitch:
	case expModeSolenoidPlusNCFn2LockSwitch:
	case expModeSolenoidPlusNOFn3LockSwitch:
	case expModeSolenoidPlusNCFn3LockSwitch:
		PORTB |= (1 << PB5); // turn on pullup for input pin
	case expModeSolenoid:
		DDRB  |= (1 << PB6);
		DDRB  |= (1 << PB7);
		PORTB |= (1 << PB7); // enable 9V SMPS
		break;
	case expModeLockLEDs:
		/* pins will already be set to outputs in expClear() */
		break;
	default:
		break;
	}
}

/*
 *
 */
void
expMSTick(void)
{
	if (msCtr < 0xff)
		msCtr++;

	switch (expMode) {
	case expModeSolenoid:
	case expModeSolenoidPlusNOCapsLockSwitch:
	case expModeSolenoidPlusNCCapsLockSwitch:
	case expModeSolenoidPlusNONumLockSwitch:
	case expModeSolenoidPlusNCNumLockSwitch:
	case expModeSolenoidPlusNOShiftLockSwitch:
	case expModeSolenoidPlusNCShiftLockSwitch:
	case expModeSolenoidPlusNOFn1LockSwitch:
	case expModeSolenoidPlusNCFn1LockSwitch:
	case expModeSolenoidPlusNOFn2LockSwitch:
	case expModeSolenoidPlusNCFn2LockSwitch:
	case expModeSolenoidPlusNOFn3LockSwitch:
	case expModeSolenoidPlusNCFn3LockSwitch:
		if (msCtr == expVal1)
			PORTB &= ~(1 << PB6); // turn off solenoid
		else if (msCtr == expVal2)
			scanResume();
		else if (solenoidQueue > 0 && msCtr > expVal2) {
			msCtr = 0;
			solenoidQueue--;
			PORTB |= (1 << PB6);  // turn on solenoid
			scanPause();
		}
		break;
	default:
		break;
	}
}

/*
 *
 */
void
expSetMode(uint8_t mode, uint8_t val1, uint8_t val2)
{
	expMode = mode;
	expVal1 = val1;
	expVal2 = val2;
}

/*
 *
 */
static void
handleFnLockSelect(bool switchState, uint8_t layer)
{
	if (switchState)
		layersDefaultLayer = layer;
	else
		layersDefaultLayer = 0;
}

/*
 *
 */
static void
handleShiftLockSelect(bool switchState)
{
	if (switchState)
		kbdSCBmp[HID_KEYBOARD_SC_LEFT_SHIFT / 8] |=
		   (1 << (HID_KEYBOARD_SC_LEFT_SHIFT % 8));
}

/*
 * This is a bit hard, as we need to look at what the OS is telling us via the
 * keyboard lock LED report, and toggle the corresponding lock key to make it
 * switch state. There's a bit of a lag between getting the new LED states too,
 * so use a counter to have a bit of a timeout before trying again.
 */
static bool
handleLEDKeyLockSelect(bool switchState, uint8_t ledMask, uint8_t scancode)
{
	static uint8_t timeout;

	/* don't do anything if we don't need to */
	if ((!switchState && !(kbdLEDs & ledMask)) ||
	    ( switchState &&  (kbdLEDs & ledMask))) {
		timeout = 0;
		return false;
	}

	if (timeout++ == 0) {
		kbdSCBmp[scancode / 8] |=  (1 << (scancode % 8));
		return true;
	}

	return false;
}

/*
 * look for new keypress edges; ignore the first four scancodes
 * (ignored/pressed/released etc.) and the last chunk which is fn keys and
 * mods.
 */
static void
processSolenoidScancodes(void)
{
	for (uint8_t i = 0; i < KBD_SC_FN1 / 8; i++) {
		uint8_t prev = kbdPrevSCBmp[i];
		uint8_t curr = kbdSCBmp[i];

		if (prev == curr)
			continue;

		/* mask to get new bits from prev -> curr */
		curr = ((prev ^ curr) & ~prev);

		if (i == 0)
			curr &= 0xf0; // get rid of first four scancodes

		/* whack the solenoid once for each set bit */
		while (curr) {
			curr &= (curr - 1);
			solenoidQueue++;
		}
	}
}

/*
 *
 */
static void
setLockLEDs(void)
{
	if (kbdLEDs & HID_KEYBOARD_LED_NUMLOCK)
		PORTB |=  (1 << PB5);
	else
		PORTB &= ~(1 << PB5);

	if (kbdLEDs & HID_KEYBOARD_LED_CAPSLOCK)
		PORTB |=  (1 << PB6);
	else
		PORTB &= ~(1 << PB6);

	if (kbdLEDs & HID_KEYBOARD_LED_SCROLLLOCK)
		PORTB |=  (1 << PB4);
	else
		PORTB &= ~(1 << PB4);
}

/*
 *
 */
static bool
switchIsSet(void)
{
	return !(PINB & (1 << PB5));
}

/*
 * returns true if it caused a change in the scan
 */
bool
expProcessScan(bool scanChanged)
{
	if (scanChanged) {
		switch (expMode) {
		case expModeSolenoid:
		case expModeSolenoidPlusNOCapsLockSwitch:
		case expModeSolenoidPlusNCCapsLockSwitch:
		case expModeSolenoidPlusNONumLockSwitch:
		case expModeSolenoidPlusNCNumLockSwitch:
		case expModeSolenoidPlusNOShiftLockSwitch:
		case expModeSolenoidPlusNCShiftLockSwitch:
		case expModeSolenoidPlusNOFn1LockSwitch:
		case expModeSolenoidPlusNCFn1LockSwitch:
		case expModeSolenoidPlusNOFn2LockSwitch:
		case expModeSolenoidPlusNCFn2LockSwitch:
		case expModeSolenoidPlusNOFn3LockSwitch:
		case expModeSolenoidPlusNCFn3LockSwitch:
			processSolenoidScancodes();
			break;
		}
	}

	switch (expMode) {
	case expModeLockLEDs:
		setLockLEDs();
		break;
	case expModeSolenoidPlusNOCapsLockSwitch:
		return handleLEDKeyLockSelect(switchIsSet(),
					      HID_KEYBOARD_LED_CAPSLOCK,
					      HID_KEYBOARD_SC_CAPS_LOCK);
	case expModeSolenoidPlusNCCapsLockSwitch:
		return handleLEDKeyLockSelect(!switchIsSet(),
					      HID_KEYBOARD_LED_CAPSLOCK,
					      HID_KEYBOARD_SC_CAPS_LOCK);
	case expModeSolenoidPlusNONumLockSwitch:
		return handleLEDKeyLockSelect(switchIsSet(),
					      HID_KEYBOARD_LED_NUMLOCK,
					      HID_KEYBOARD_SC_NUM_LOCK);
	case expModeSolenoidPlusNCNumLockSwitch:
		return handleLEDKeyLockSelect(!switchIsSet(),
					      HID_KEYBOARD_LED_NUMLOCK,
					      HID_KEYBOARD_SC_NUM_LOCK);
		break;
	case expModeSolenoidPlusNOShiftLockSwitch:
		handleShiftLockSelect(switchIsSet());
		break;
	case expModeSolenoidPlusNCShiftLockSwitch:
		handleShiftLockSelect(!switchIsSet());
		break;
	case expModeSolenoidPlusNOFn1LockSwitch:
		handleFnLockSelect(switchIsSet(), 1);
		break;
	case expModeSolenoidPlusNCFn1LockSwitch:
		handleFnLockSelect(!switchIsSet(), 1);
		break;
	case expModeSolenoidPlusNOFn2LockSwitch:
		handleFnLockSelect(switchIsSet(), 2);
		break;
	case expModeSolenoidPlusNCFn2LockSwitch:
		handleFnLockSelect(!switchIsSet(), 2);
		break;
	case expModeSolenoidPlusNOFn3LockSwitch:
		handleFnLockSelect(switchIsSet(), 3);
		break;
	case expModeSolenoidPlusNCFn3LockSwitch:
		handleFnLockSelect(!switchIsSet(), 3);
		break;
	}

	return false;
}

/*
 *
 */
void
expStore(void)
{
	eeprom_update_byte((uint8_t *)EEP_EXP_MODE, expMode);
	eeprom_update_byte((uint8_t *)EEP_EXP_VAL1, expVal1);
	eeprom_update_byte((uint8_t *)EEP_EXP_VAL2, expVal2);
}

/*
 *
 */
void
expLoad(void)
{
	uint8_t mode = eeprom_read_byte((uint8_t *)EEP_EXP_MODE);
	if (mode >= expModeEND)
		mode = expModeDisabled;

	expSetMode(mode,
		   eeprom_read_byte((uint8_t *)EEP_EXP_VAL1),
		   eeprom_read_byte((uint8_t *)EEP_EXP_VAL2));
}
