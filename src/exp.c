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

/* 0-bit = reported lock state (through LED report)
 * 1-bit = last switch state
 * 2-bit = have run logic at least once since reset
 */
static uint8_t lockState;

uint8_t	       expMode;
uint8_t	       expVal1;	// meaning of val 1 & 2 is depending on mode
uint8_t	       expVal2;

static uint8_t switchLockLogic(uint8_t switchState);
static void    handleFnLockSelect(uint8_t switchState);

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
	lockState &= ~(1 << 2); // reset `logic run flag' (see above)

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
		else if (solenoidQueue > 0 && msCtr >= expVal2) {
			msCtr = 0;
			solenoidQueue--;
			PORTB |= (1 << PB6);  // turn on solenoid
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
expKeyPositiveEdge(void)
{
	solenoidQueue++;
}

/*
 *
 */
uint8_t
switchLockLogic(uint8_t switchState)
{
	/* run on switch edges, or if we haven't run since reset */
	if ((!switchState &&  (lockState & (1 << 1))) ||
	    ( switchState && !(lockState & (1 << 1))) ||
	    (!(lockState & (1 << 2)))) {
		lockState |= (1 << 2);
		if (switchState)
			lockState |=  (1 << 1);
		else
			lockState &= ~(1 << 1);

		if (switchState && !(lockState & (1 << 0)))
			return 1;
		else if (!switchState && (lockState & (1 << 0)))
			return 1;
	}

	return 0;
}

void
handleFnLockSelect(uint8_t switchState)
{
	if (switchState) {
		switch (expMode) {
		case expModeSolenoidPlusNOFn1LockSwitch:
		case expModeSolenoidPlusNCFn1LockSwitch:
			layersDefaultLayer = 1;
			break;
		case expModeSolenoidPlusNOFn2LockSwitch:
		case expModeSolenoidPlusNCFn2LockSwitch:
			layersDefaultLayer = 2;
			break;
		case expModeSolenoidPlusNOFn3LockSwitch:
		case expModeSolenoidPlusNCFn3LockSwitch:
			layersDefaultLayer = 3;
			break;
		}
	} else
		layersDefaultLayer = 0;
}

/*
 *
 */
void
expPostProcessStdKbdReport(USB_KeyboardReport_Data_t *report,
			   uint8_t usedKeyCodes)
{
	switch (expMode) {
	case expModeSolenoidPlusNOCapsLockSwitch:
		if (switchLockLogic(!(PINB & (1 << PB5))))
			report->KeyCode[usedKeyCodes] =
			   HID_KEYBOARD_SC_CAPS_LOCK;
		break;
	case expModeSolenoidPlusNCCapsLockSwitch:
		if (switchLockLogic(PINB & (1 << PB5)))
			report->KeyCode[usedKeyCodes] =
			   HID_KEYBOARD_SC_CAPS_LOCK;
		break;
	case expModeSolenoidPlusNONumLockSwitch:
		if (switchLockLogic(!(PINB & (1 << PB5))))
			report->KeyCode[usedKeyCodes] =
			   HID_KEYBOARD_SC_NUM_LOCK;
		break;
	case expModeSolenoidPlusNCNumLockSwitch:
		if (switchLockLogic(PINB & (1 << PB5)))
			report->KeyCode[usedKeyCodes] =
			   HID_KEYBOARD_SC_NUM_LOCK;
		break;
	case expModeSolenoidPlusNOShiftLockSwitch:
		if (!(PINB & (1 << PB5)))
			report->Modifier += HID_KEYBOARD_MODIFIER_LEFTSHIFT;
		break;
	case expModeSolenoidPlusNCShiftLockSwitch:
		if (PINB & (1 << PB5))
			report->Modifier += HID_KEYBOARD_MODIFIER_LEFTSHIFT;
		break;
	case expModeSolenoidPlusNOFn1LockSwitch:
	case expModeSolenoidPlusNOFn2LockSwitch:
	case expModeSolenoidPlusNOFn3LockSwitch:
		handleFnLockSelect(!(PINB & (1 << PB5)));
		break;
	case expModeSolenoidPlusNCFn1LockSwitch:
	case expModeSolenoidPlusNCFn2LockSwitch:
	case expModeSolenoidPlusNCFn3LockSwitch:
		handleFnLockSelect(PINB & (1 << PB5));
		break;
	default:
		break;
	}
}

/*
 *
 */
void
expPostProcessNKROKbdReport(NKROReport *report)
{
	switch (expMode) {
	case expModeSolenoidPlusNOCapsLockSwitch:
		if (switchLockLogic(!(PINB & (1 << PB5))))
			report->codeBmp[HID_KEYBOARD_SC_CAPS_LOCK / 8] |=
			   (1 << (HID_KEYBOARD_SC_CAPS_LOCK % 8));
		break;
	case expModeSolenoidPlusNCCapsLockSwitch:
		if (switchLockLogic(PINB & (1 << PB5)))
			report->codeBmp[HID_KEYBOARD_SC_CAPS_LOCK / 8] |=
			   (1 << (HID_KEYBOARD_SC_CAPS_LOCK % 8));
		break;
	case expModeSolenoidPlusNONumLockSwitch:
		if (switchLockLogic(!(PINB & (1 << PB5))))
			report->codeBmp[HID_KEYBOARD_SC_NUM_LOCK / 8] |=
			   (1 << (HID_KEYBOARD_SC_NUM_LOCK % 8));
		break;
	case expModeSolenoidPlusNCNumLockSwitch:
		if (switchLockLogic(PINB & (1 << PB5)))
			report->codeBmp[HID_KEYBOARD_SC_NUM_LOCK / 8] |=
			   (1 << (HID_KEYBOARD_SC_NUM_LOCK % 8));
		break;
	case expModeSolenoidPlusNOShiftLockSwitch:
		if (!(PINB & (1 << PB5)))
			report->modifiers |=
			   (1 << (HID_KEYBOARD_SC_LEFT_SHIFT - 0xe0));
		break;
	case expModeSolenoidPlusNCShiftLockSwitch:
		if (PINB & (1 << PB5))
			report->modifiers |=
			   (1 << (HID_KEYBOARD_SC_LEFT_SHIFT - 0xe0));
		break;
	case expModeSolenoidPlusNOFn1LockSwitch:
	case expModeSolenoidPlusNOFn2LockSwitch:
	case expModeSolenoidPlusNOFn3LockSwitch:
		handleFnLockSelect(!(PINB & (1 << PB5)));
		break;
	case expModeSolenoidPlusNCFn1LockSwitch:
	case expModeSolenoidPlusNCFn2LockSwitch:
	case expModeSolenoidPlusNCFn3LockSwitch:
		handleFnLockSelect(PINB & (1 << PB5));
		break;
	default:
		break;
	}
}

/*
 *
 */
void
expSetLockLEDs(uint8_t leds)
{
	switch (expMode) {
	case expModeLockLEDs:
		if (leds & HID_KEYBOARD_LED_NUMLOCK)
			PORTB |=  (1 << PB5);
		else
			PORTB &= ~(1 << PB5);

		if (leds & HID_KEYBOARD_LED_CAPSLOCK)
			PORTB |=  (1 << PB6);
		else
			PORTB &= ~(1 << PB6);

		if (leds & HID_KEYBOARD_LED_SCROLLLOCK)
			PORTB |=  (1 << PB4);
		else
			PORTB &= ~(1 << PB4);
		break;
	case expModeSolenoidPlusNOCapsLockSwitch:
	case expModeSolenoidPlusNCCapsLockSwitch:
		if (leds & HID_KEYBOARD_LED_CAPSLOCK)
			lockState |=  (1 << 0);
		else
			lockState &= ~(1 << 0);
		break;
	case expModeSolenoidPlusNONumLockSwitch:
	case expModeSolenoidPlusNCNumLockSwitch:
		if (leds & HID_KEYBOARD_LED_NUMLOCK)
			lockState |=  (1 << 0);
		else
			lockState &= ~(1 << 0);
		break;
	default:
		break;
	}
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
	expMode = eeprom_read_byte((uint8_t *)EEP_EXP_MODE);
	if (expMode >= expModeEND)
		expMode = expModeDisabled;

	expVal1 = eeprom_read_byte((uint8_t *)EEP_EXP_VAL1);
	expVal2 = eeprom_read_byte((uint8_t *)EEP_EXP_VAL2);
}
