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
#include "diag.h"

#if defined (__AVR_ATmega8U2__)
void (*bootloader)(void) = (void *)0x0800;
#elif defined (__AVR_ATmega32U2__)
void (*bootloader)(void) = (void *)0x1800;
#endif

static DiagReportState	diagReportState = dr1stCols;

static void	diagReportInfo(uint8_t *reportData);
static void	diagReportKbd(uint8_t *reportData,
			      uint8_t startCol,
			      uint8_t endCol);
static void	diagReportVref(uint8_t *reportData);
static void	diagReportScanCodes(uint8_t *reportData);
static void	diagReportLayerConditions(uint8_t *reportData);
static void	diagReportVersion(uint8_t *reportData);
static void	diagReportExpMode(uint8_t *reportData);
static void	diagReportKbdColSkips(uint8_t *reportData);
static void	diagSetScanCode(uint8_t layer,
				uint8_t col,
				uint8_t row,
				uint8_t sc);
static void	diagSetLayerCondition(uint8_t conditionIdx, uint8_t val);
static void	diagSetExpMode(uint8_t mode, uint8_t val1, uint8_t val2);
static void	diagSetKbdColSkips(const uint8_t *reportData);

/*
 *
 */
void
diagReceiveReport(const uint8_t *reportData)
{
	DiagReportCommand cmd = reportData[0];

	switch (cmd) {
	case dcSetState:
		diagReportState = reportData[1];
		break;
	case dcSetVref:
		vrefSet(reportData[1] + (reportData[2] << 8));
		break;
	case dcJumpBootloader:
		if (reportData[1])
			bootloader();
		break;
	case dcStoreVref:
		if (reportData[1])
			vrefStore();
		break;
	case dcAutoCalibrate:
		if (reportData[1])
			kbdCalibrate();
		break;
	case dcSetScanCode:
		diagSetScanCode(reportData[1] >> 6, reportData[1] & 0x3f,
				reportData[2], reportData[3]);
		break;
	case dcSetLayerCondition:
		diagSetLayerCondition(reportData[1], reportData[2]);
		break;
	case dcLoadScanCodes:
		if (reportData[1])
			layersLoad();
		break;
	case dcStoreScanCodes:
		if (reportData[1])
			layersStore();
		break;
	case dcSetExpMode:
		diagSetExpMode(reportData[1], reportData[2], reportData[3]);
		break;
	case dcStoreExpMode:
		if (reportData[1])
			expStore();
		break;
	case dcSetKbdColSkips:
		diagSetKbdColSkips(reportData);
		break;
	case dcStoreKbdColSkips:
		if (reportData[1])
			kbdStoreColSkips();
		break;
	}
}

/*
 *
 */
void
diagFillReport(uint8_t *reportData)
{
	/* zero report */
	memset(reportData, 0x00, GENERIC_REPORT_SIZE);

	/* first byte of report is reporting state */
	reportData[0] = diagReportState;

	switch (diagReportState) {
	case drInfo:
		diagReportInfo(reportData);
		break;
	case dr1stCols:
		diagReportKbd(reportData, 0, 6);
		diagReportState = dr2ndCols;
		break;
	case dr2ndCols:
#if KBD_COLS == 12
                diagReportKbd(reportData, 7, 11);
#elif KBD_COLS == 16 || KBD_COLS == 23
		diagReportKbd(reportData, 7, 13);
#endif
		diagReportState = dr3rdCols;
		break;
	case dr3rdCols:
#if KBD_COLS == 12
		diagReportState = dr1stCols;
#elif KBD_COLS == 16
		diagReportKbd(reportData, 14, 15);
		diagReportState = dr1stCols;
#elif KBD_COLS == 23
		diagReportKbd(reportData, 14, 20);
		diagReportState = dr4thCols;
#else
#	error "kbd columns not 12, 16 or 23"
#endif
		break;
	case dr4thCols:
#if KBD_COLS == 12 || KBD_COLS == 16
#elif KBD_COLS == 23
		diagReportKbd(reportData, 21, 23);
		diagReportState = dr4thCols;
#else
#	error "kbd columns not 16 or 23"
#endif
		diagReportState = dr1stCols;
		break;
	case drVref:
		diagReportVref(reportData);
		break;
	case drScanCodes:
		diagReportScanCodes(reportData);
		break;
	case drLayerConditions:
		diagReportLayerConditions(reportData);
		break;
	case drVersion:
		diagReportVersion(reportData);
		break;
	case drExpMode:
		diagReportExpMode(reportData);
		break;
	case drKbdColSkips:
		diagReportKbdColSkips(reportData);
		break;
	default:
		break;
	}
}

/*
 *
 */
void
diagReportInfo(uint8_t *reportData)
{
	reportData[1] = 4; // number of layers
	reportData[2] = LAYERS_NUM_CONDITIONS;
	reportData[3] = KBD_COLS;
	reportData[4] = KBD_ROWS;

#if defined(BEAMSPRING)
	reportData[5] = dktBeamspring;
#elif defined(BEAMSPRING_DISPLAYWRITER)
	reportData[5] = dktBeamspringDisplaywriter;
#elif defined(MODEL_F)
	reportData[5] = dktModelF;
#else
#	error "keyboard type not known!"
#endif
}

/*
 *
 */
void
diagReportKbd(uint8_t *reportData, uint8_t startCol, uint8_t endCol)
{
	/* data is sent column-major, one column per byte (even on 4-row kbd) */
	uint8_t currByte = 1;

	for (uint8_t col = startCol; col <= endCol; col++, currByte++)
		for (uint8_t row = 0; row < KBD_ROWS; row++)
			if (kbd[col][row] > KBD_DEBOUNCE_THRESH)
				reportData[currByte] |= (1 << row);
}

/*
 *
 */
void
diagReportVref(uint8_t *reportData)
{
	uint16_t vref = vrefGet();
	reportData[1] = vref & 0xff;
	reportData[2] = vref >> 8;
}

/*
 *
 */
void
diagReportScanCodes(uint8_t *reportData)
{
	static uint8_t currLayer = 0;
	static uint8_t (*currMtx)[KBD_COLS][KBD_ROWS] = &layerMatrix0;
	static uint8_t currSlice = 0;
	static uint8_t currCol = 0;

	/* first byte of report has layer in top two bits, the current row slice
	 * in the next bit (we do four rows at a time), and column in the
	 * bottom 5.
	 */
	reportData[1] = (currLayer << 6) | (currSlice << 5) | currCol;

	if (KBD_ROWS == 4 || currSlice == 0)
		for (uint8_t i = 0; i < 4; i++)
			reportData[i + 2] = (*currMtx)[currCol][i];
	else if (KBD_ROWS == 8 && currSlice == 1)
		for (uint8_t i = 4; i < 8; i++)
			reportData[i - 2] = (*currMtx)[currCol][i];

	currCol++;
	if (currCol == KBD_COLS) {
		currCol = 0;

#if KBD_ROWS == 8
		currSlice++;
		if (currSlice == 1)
			return;
		else
			currSlice = 0;
#endif

		currLayer++;
		if (currLayer == 4)
			currLayer = 0;
		switch (currLayer) {
		case 0:
			currMtx = &layerMatrix0;
			break;
		case 1:
			currMtx = &layerMatrix1;
			break;
		case 2:
			currMtx = &layerMatrix2;
			break;
		case 3:
			currMtx = &layerMatrix3;
			break;
		}
	}
}

/*
 *
 */
void
diagReportLayerConditions(uint8_t *reportData)
{
	static uint8_t currSet = 0; // sent in two halves

	reportData[1] = currSet;

	memcpy(&reportData[2],
	       &layerConditions[currSet * (LAYERS_NUM_CONDITIONS / 2)],
	       LAYERS_NUM_CONDITIONS / 2);

	currSet++;
	if (currSet == 2)
		currSet = 0;
}

/*
 *
 */
void
diagReportVersion(uint8_t *reportData)
{
	char *c = VERSION;
	uint8_t i = 1;

	while ((reportData[i++] = *c++) != '\0' && i < 7)
		;
	reportData[i] = '\0';
}

/*
 *
 */
void
diagReportExpMode(uint8_t *reportData)
{
	reportData[1] = expMode;
	reportData[2] = expVal1;
	reportData[3] = expVal2;
}

/*
 *
 */
void
diagReportKbdColSkips(uint8_t *reportData)
{
	memcpy(&reportData[1], kbdColSkips, sizeof(kbdColSkips));
}

/*
 *
 */
void
diagSetScanCode(uint8_t layer, uint8_t col, uint8_t row, uint8_t sc)
{
	if (col >= 0 && col < KBD_COLS && row >= 0 && row < KBD_ROWS)
		(*layersMatrix(layer))[col][row] = sc;
}

/*
 *
 */
void
diagSetLayerCondition(uint8_t conditionIdx, uint8_t val)
{
	if (conditionIdx >= 0 && conditionIdx < LAYERS_NUM_CONDITIONS)
		layerConditions[conditionIdx] = val;
}

/*
 *
 */
void
diagSetExpMode(uint8_t mode, uint8_t val1, uint8_t val2)
{
	expMode = mode;
	expVal1 = val1;
	expVal2 = val2;

	expReset();
}

/*
 *
 */
void
diagSetKbdColSkips(const uint8_t *reportData)
{
	memcpy(kbdColSkips, &reportData[1], sizeof(kbdColSkips));
}
