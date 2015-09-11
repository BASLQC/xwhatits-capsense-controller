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
 * (1 byte numMakeCmds)
 * (1 byte numBreakCmds)
 * ...then repeated pairs of:
 * (1 byte cmd)
 * (1 byte val)
 *
 * If it's the last macro, it's followed by a null byte.
 */
#define MACROS_OFFS_NUMMAKECMDS  3
#define MACROS_OFFS_NUMBREAKCMDS (MACROS_OFFS_NUMMAKECMDS + 1)
#define MACROS_OFFS_FIRSTMAKECMD (MACROS_OFFS_NUMBREAKCMDS + 1)

typedef enum {
	msIdle
} MacroState;

MacroDef macrosDefs[MACROS_NUM_MACROS];

//static MacroState state;
static int8_t     currMacro;
static uint8_t    currPos;

static uint8_t *macrosLoadDef(uint8_t macroIdx, uint8_t *addr);
//static void     macrosPlayMacro(NKROReport *report, NKROReport *prevReport);

/*
 *
 */
void
macrosInit(void)
{
	currMacro = -1;
	macrosLoad();
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
 * returns pointer to start of next macro in EEPROM
 */
uint8_t *
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
	addr += eeprom_read_byte(addr) * 2;
	addr++;
	addr += eeprom_read_byte(addr) * 2;

	return addr;
}

/*
 *
 */
void
macrosPostProcessNKROReport(NKROReport *report, NKROReport *prevReport)
{
	DDRB  |= (1 << PB4);
	DDRB  |= (1 << PB5);
	DDRB  |= (1 << PB6);
	PORTB &= (1 << PB4); // scr
	PORTB &= (1 << PB5); // num
	PORTB &= (1 << PB6); // caps

	//if (currMacro != -1)
	//	macrosPlayMacro(report, prevReport);

	for (uint8_t i = 0; i < MACROS_NUM_MACROS; i++) {
		uint8_t sc = macrosDefs[i].scancode;
		PORTB |= (1 << PB6);
		if (sc == KBD_SC_IGNORED)
			break;

		/* match scancode */
		if (!(report->codeBmp[sc / 8] & (1 << (sc % 8))))
			continue;

		PORTB |= (1 << PB5);

		/* match modifiers, doing cool thing copied from Soarer
		 * where we take care of Shift matching either LShift or
		 * RShift while only using two bytes... */
		uint8_t mods = report->modifiers;

		/* cool thing (see above). maybe soarer does this properly. */
		mods |= ((mods >> 4) &
			 (macrosDefs[i].modStates >> 4 &
			  macrosDefs[i].modStates));
		if ((report->modifiers       & macrosDefs[i].modMask) !=
		    (macrosDefs[i].modStates & macrosDefs[i].modMask))
			continue;

		currMacro = i;
		currPos = 0;
		break;
	}
}

/*
 *
 */
void
macrosPlayMakeMacro(NKROReport *report, NKROReport *prevReport)
{
}
