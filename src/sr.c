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
#include "sr.h"

/*
 *
 */
void
srInit(void)
{
	SR_DDR |= (1 << SR_D_OP);
	SR_DDR |= (1 << SR_C_OP);
	SR_DDR |= (1 << SR_OE_OP);

#if defined(SR_STCP_OP)
	SR_DDR |= (1 << SR_STCP_OP);
#endif
}

/*
 *
 */
void
srClear(void)
{
	SR_PORT &= ~(1 << SR_C_OP);
	SR_PORT &= ~(1 << SR_D_OP);
	for (uint8_t i = 0; i < KBD_COLS + 1; i++) {
		SR_PORT |=  (1 << SR_C_OP);
		SR_PORT &= ~(1 << SR_C_OP);
	}
}

/*
 *
 */
void
srSet(uint8_t pos)
{
	SR_PORT &= ~(1 << SR_C_OP);
	SR_PORT &= ~(1 << SR_D_OP);
	SR_PORT |=  (1 << SR_D_OP);
	SR_PORT |=  (1 << SR_C_OP);
	SR_PORT &= ~(1 << SR_C_OP);
	SR_PORT &= ~(1 << SR_D_OP);

	for (uint8_t i = 0; i < pos; i++) {
		SR_PORT |=  (1 << SR_C_OP);
		SR_PORT &= ~(1 << SR_C_OP);
	}
}
