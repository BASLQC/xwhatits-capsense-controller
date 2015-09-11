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

static uint8_t	msCtr;
static uint8_t	solenoidQueue;
uint8_t		expMode;
uint8_t		expVal1;	// meaning of val 1 & 2 is depending on mode
uint8_t		expVal2;

/*
 *
 */
void
expInit(void)
{
	DDRB |= (1 << PB4);
	DDRB |= (1 << PB5);
	DDRB |= (1 << PB6);
	DDRB |= (1 << PB7);
}

/*
 *
 */
void
expClear(void)
{
	PORTB &= ~(1 << PB4);
	PORTB &= ~(1 << PB5);
	PORTB &= ~(1 << PB6);
	PORTB &= ~(1 << PB7);

	msCtr = 0;
	solenoidQueue = 0;
}

/*
 *
 */
void
expReset(void)
{
	expClear();

	switch (expMode) {
	case expModeSolenoid:
		PORTB |= (1 << PB7);	// enable 9V SMPS
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
void
expSetLockLEDs(uint8_t leds)
{
	if (expMode != expModeLockLEDs)
		return;

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
