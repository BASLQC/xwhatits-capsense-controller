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
#ifndef KBD_DEFS_H
#define KBD_DEFS_H

#if defined(BEAMSPRING)
#	define KBD_COLS 23
#	define KBD_ROWS 4
#	if BOARD_REV <= 2
#		define KBD_ROW1_PIN PINB
#		define KBD_ROW2_PIN PINB
#		define KBD_ROW3_PIN PINB
#		define KBD_ROW4_PIN PINB
#		define KBD_ROW1_IP  PB0
#		define KBD_ROW2_IP  PB1
#		define KBD_ROW3_IP  PB2
#		define KBD_ROW4_IP  PB3
#	elif BOARD_REV <= 4
#		define KBD_ROW1_PIN PIND
#		define KBD_ROW2_PIN PIND
#		define KBD_ROW3_PIN PIND
#		define KBD_ROW4_PIN PIND
#		define KBD_ROW1_IP  PD0
#		define KBD_ROW2_IP  PD1
#		define KBD_ROW3_IP  PD2
#		define KBD_ROW4_IP  PD3
#	else
#		error "BOARD_REV not understood"
#	endif
#elif defined(MODEL_F)
#	define KBD_COLS     16
#	define KBD_ROWS     8
#	define KBD_ROW1_PIN PIND
#	define KBD_ROW2_PIN PIND
#	define KBD_ROW3_PIN PIND
#	define KBD_ROW4_PIN PIND
#	define KBD_ROW5_PIN PINC
#	define KBD_ROW6_PIN PINC
#	define KBD_ROW7_PIN PINC
#	define KBD_ROW8_PIN PINC
#	define KBD_ROW1_IP  PD3
#	define KBD_ROW2_IP  PD2
#	define KBD_ROW3_IP  PD1
#	define KBD_ROW4_IP  PD0
#	define KBD_ROW5_IP  PC7
#	define KBD_ROW6_IP  PC6
#	define KBD_ROW7_IP  PC5
#	define KBD_ROW8_IP  PC4
#elif defined(BEAMSPRING_DISPLAYWRITER)
#	define KBD_COLS     12
#	define KBD_ROWS     8
#	define KBD_ROW1_PIN PINC
#	define KBD_ROW2_PIN PIND
#	define KBD_ROW3_PIN PINC
#	define KBD_ROW4_PIN PIND
#	define KBD_ROW5_PIN PINC
#	define KBD_ROW6_PIN PIND
#	define KBD_ROW7_PIN PINC
#	define KBD_ROW8_PIN PIND
#	define KBD_ROW1_IP  PC7
#	define KBD_ROW2_IP  PD0
#	define KBD_ROW3_IP  PC6
#	define KBD_ROW4_IP  PD1
#	define KBD_ROW5_IP  PC5
#	define KBD_ROW6_IP  PD2
#	define KBD_ROW7_IP  PC4
#	define KBD_ROW8_IP  PD3
#else
#	error "BOARD_TYPE not specified in makefile"
#endif

#if defined(BEAMSPRING) || defined(BEAMSPRING_DISPLAYWRITER)
#	define KBD_ACTIVE_LOW
#elif defined(MODEL_F)
#	define KBD_ACTIVE_HIGH
#endif

#define KBD_SC_IGNORED       0x00
#define KBD_SC_CAL_HI        0x01
#define KBD_SC_CAL_LO        0x02
#define KBD_SC_SYSTEM_POWER  0xA5
#define KBD_SC_MEDIA_PLAY    0xA8
#define KBD_SC_WEB_BOOKMARKS 0xBD
#define KBD_SC_FN1           0xD0
#define KBD_SC_FN2           0xD1
#define KBD_SC_FN3           0xD2
#define KBD_SC_SELECT_0      0xD8
#define KBD_SC_SELECT_1      0xD9
#define KBD_SC_SELECT_2      0xDA
#define KBD_SC_SELECT_3      0xDB

#endif
