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
#ifndef DIAG_TYPES_H
#define DIAG_TYPES_H

typedef enum {
	drNothing,
	drInfo,
	dr1stCols,
	dr2ndCols,
	dr3rdCols,
	dr4thCols,
	drVref,
	drScanCodes,
	drLayerConditions,
	drVersion,
	drExpMode,
	drKbdColSkips,
	drEEPROM,
        drDebug,
        drMacroEEPROMSize,
        drMacros
} DiagReportState;

typedef enum {
	dcSetState,
	dcSetVref,
	dcJumpBootloader,
	dcStoreVref,
	dcAutoCalibrate,
	dcSetScanCode,
	dcSetLayerCondition,
	dcLoadScanCodes,
	dcStoreScanCodes,
	dcSetExpMode,
	dcStoreExpMode,
	dcSetKbdColSkips,
	dcStoreKbdColSkips,
	dcWriteEEPROMByte,
	dcSetScanEnabled,
	dcWriteMacroSegment,
	dcLoadMacros
} DiagReportCommand;

typedef enum {
	dktInvalid,
	dktBeamspring,
	dktBeamspringDisplaywriter,
	dktModelF
} DiagKeyboardType;

#endif
