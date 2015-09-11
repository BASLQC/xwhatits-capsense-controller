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
#ifndef SR_H
#define SR_H

#include <avr/io.h>

/* row drive shift registers */

/* pin definitions */
#if defined(BEAMSPRING)
#       define SR_DDR   DDRC
#       define SR_PORT  PORTC
#       define SR_D_OP  PC4
#       define SR_C_OP  PC5
#       define SR_OE_OP PC6
#	if BOARD_REV == 4
#               define SR_STCP_OP PC7
#       endif
#elif defined(BEAMSPRING_DISPLAYWRITER) || defined(MODEL_F)
#       define SR_DDR     DDRD
#	define SR_PORT    PORTD
#	define SR_D_OP    PD4
#	define SR_C_OP    PD7
#	define SR_OE_OP   PD5
#	define SR_STCP_OP PD6
#endif

/* shift register specifics */
#if defined(BEAMSPRING) && BOARD_REV <= 3
/* 	4094 shift registers have active-high OE */
#	define srEnable()  (SR_PORT |=  (1 << SR_OE_OP))
#	define srDisable() (SR_PORT &= ~(1 << SR_OE_OP))
#elif defined(BEAMSPRING_DISPLAYWRITER) || defined(MODEL_F)  || \
   (defined(BEAMSPRING) && BOARD_REV == 4)
/* 	595 shift registers have active-low OE, but we use the strobe instead */
#	define srEnable()           \
   do {                             \
	   SR_PORT |=  (1 << SR_STCP_OP);  \
	   SR_PORT &= ~(1 << SR_STCP_OP);  \
   } while (0)                           
#	define srDisable()     /* nothing */
#else
#	error "BOARD_TYPE not specified in makefile"
#endif

void	srInit(void);
void	srClear(void);
void	srSet(uint8_t pos);

#endif
