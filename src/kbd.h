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
#ifndef KBD_H
#define KBD_H

#include <LUFA/Drivers/USB/USB.h>
#include <stdlib.h>
#include "descriptors.h"
#include "eeprom.h"
#include "exp.h"
#include "kbd_defs.h"
#include "layers.h"
#include "macros.h"
#include "reports.h"
#include "scan.h"
#include "vref.h"

extern uint8_t kbdSCBmp[29];
extern uint8_t kbdPrevSCBmp[sizeof(kbdSCBmp)];
extern uint8_t kbdLEDs;
extern uint8_t kbdColSkips[3];

void kbdInit(void);
void kbdCalibrate(void);
bool kbdUpdateSCBmp(void);
void kbdFillReport(USB_KeyboardReport_Data_t *kbdReport);
void kbdFillNKROReport(NKROReport *report);
bool kbdFillSystemReport(ExtrakeySystemReport *report);
bool kbdFillConsumerReport(ExtrakeyConsumerReport *report);
void kbdReceiveReport(const void *report);
void kbdReceiveNKROReport(const void *report);
bool kbdWantsWakeup(void);
void kbdStoreColSkips(void);
void kbdLoadColSkips(void);

#endif
