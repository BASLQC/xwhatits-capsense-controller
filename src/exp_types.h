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
#ifndef EXP_TYPES_H
#define EXP_TYPES_H

enum ExpMode {
	expModeDisabled,
	expModeSolenoid,
	expModeLockLEDs,
	expModeSolenoidPlusNOCapsLockSwitch,
	expModeSolenoidPlusNCCapsLockSwitch,
	expModeSolenoidPlusNONumLockSwitch,
	expModeSolenoidPlusNCNumLockSwitch,
	expModeSolenoidPlusNOShiftLockSwitch,
	expModeSolenoidPlusNCShiftLockSwitch,
	expModeSolenoidPlusNOFn1LockSwitch,
	expModeSolenoidPlusNCFn1LockSwitch,
	expModeSolenoidPlusNOFn2LockSwitch,
	expModeSolenoidPlusNCFn2LockSwitch,
	expModeSolenoidPlusNOFn3LockSwitch,
	expModeSolenoidPlusNCFn3LockSwitch,
	expModeEND
};

#endif
