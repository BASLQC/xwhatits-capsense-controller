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
#include "scan.h"

volatile uint8_t scanTick;
volatile int8_t  scanState[KBD_COLS][KBD_ROWS];

static uint8_t currCol;

/*
 *
 */
void
scanInit(void)
{
	memset(scanState, 0, sizeof(scanState[0][0]) * KBD_COLS * KBD_ROWS);
}

/*
 *
 */
void
scanEnable(void)
{
	/* set up primary timer for column-scan interrupt, 100us per column */
	TCCR0A |= (1 << WGM01);              // CTC mode
	OCR0A   = 25;                        // 100us at 16MHz/64
	TIMSK0 |= (1 << OCIE0A);             // enable compare-match interrupt
	TCCR0B |= (1 << CS01) | (1 << CS00); // start timer, clk/64

	/* secondary timer is kicked off at the end of the primary timer's
	 * interrupt, to read the results of the column scan after a short
	 * delay
	 */
	TCCR1A  = 0;             // normal mode
	OCR1A   = 8;             // 4us at 16MHz/8
	TIMSK1 |= (1 << OCIE1A); // enable compare-match interrupt
}

/*
 *
 */
void
scanDisable(void)
{
	/* disable compare-match interrupts */
	TIMSK0 &= ~(1 << OCIE0A);
	TIMSK1 &= ~(1 << OCIE1A);

	/* stop timers */
	TCCR0B = 0;
	TCCR1B = 0;

	/* clear scan state */
	memset(scanState, 0, sizeof(scanState[0][0]) * KBD_COLS * KBD_ROWS);
}

/*
 * disables just primary column-strobe timer
 */
void
scanPause(void)
{
	TCCR0B = 0;
	TCNT0  = 0;
}

/*
 *
 */
void
scanResume(void)
{
	TCCR0B = (1 << CS01) | (1 << CS00);
}

/*
 * Sets currCol to the next column not in kbdColSkips; if all columns are
 * skipped, sets it to KBD_COLS which will be caught in the interrupt.
 */
static void
findNextColumn(void)
{
	uint8_t maxLoops = KBD_COLS;
	do {
		currCol++;
		if (currCol >= KBD_COLS) {
			currCol = 0;
			scanTick++;
		}

		if (!(kbdColSkips[currCol / 8] & (1 << (currCol % 8))))
			return;

	} while (--maxLoops);

	currCol = KBD_COLS;
}

/*
 * primary timed column-scan interrupt
 */
ISR(TIMER0_COMPA_vect)
{
	/* select column */
	srDisable();
	srSet(currCol);
	srEnable();

	/* start secondary timer to read results in a little bit */
	TCCR1B = (1 << CS11); // clk/8
}

/*
 * secondary row-read interrupt
 */
ISR(TIMER1_COMPA_vect)
{
	uint8_t scan[KBD_ROWS] = { 0 };

	if (currCol < KBD_COLS) {
		/* read results */
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
			if (scan[i]) {
				scanState[currCol][i]--;
				if (scanState[currCol][i] < SCAN_DB_THRESH)
					scanState[currCol][i] = 0;
			} else {
				scanState[currCol][i]++;
				if (scanState[currCol][i] >= SCAN_DB_THRESH)
					scanState[currCol][i] = SCAN_DB_TOP;
			}
		}
	}

	/* deselect column */
	srDisable();
	srClear();
	srEnable();

	findNextColumn();

	/* stop and reset timer before leaving interrupt */
	TCCR1B = 0;
	TCNT1  = 0;
}
