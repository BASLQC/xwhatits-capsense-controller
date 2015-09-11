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
#include <LUFA/Drivers/USB/USB.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <string.h>
#include "descriptors.h"
#include "diag.h"
#include "exp.h"
#include "kbd.h"
#include "macros.h"
#include "sr.h"
#include "vref.h"

static bool updateKeyboardReport = true;
static bool updateNKROReport     = true;
static bool updateSystemReport   = true;
static bool updateConsumerReport = true;

uint8_t prevGenericReport[GENERIC_REPORT_SIZE];

USB_ClassInfo_HID_Device_t keyboardHIDIface = {
	.Config = {
		.InterfaceNumber = KEYBOARD_INTERFACE,
		.ReportINEndpoint = {
			.Address = KEYBOARD_IN_EPADDR,
			.Size = KBD_EPSIZE,
			.Banks = 1,
		},
		.PrevReportINBuffer = NULL,
		.PrevReportINBufferSize = sizeof(USB_KeyboardReport_Data_t)
	}
};

USB_ClassInfo_HID_Device_t genericHIDIface = {
	.Config = {
		.InterfaceNumber = GENERIC_INTERFACE,
		.ReportINEndpoint = {
			.Address = GENERIC_IN_EPADDR,
			.Size = GENERIC_EPSIZE,
			.Banks = 1,
		},
		.PrevReportINBuffer = &prevGenericReport,
		.PrevReportINBufferSize = sizeof(prevGenericReport)
	}
};

USB_ClassInfo_HID_Device_t nkroHIDIface = {
	.Config = {
		.InterfaceNumber = NKRO_INTERFACE,
		.ReportINEndpoint = {
			.Address = NKRO_IN_EPADDR,
			.Size = NKRO_EPSIZE,
			.Banks = 1,
		},
		.PrevReportINBuffer = NULL,
		.PrevReportINBufferSize = sizeof(NKROReport)
	}
};

USB_ClassInfo_HID_Device_t extrakeyHIDIface = {
	.Config = {
		.InterfaceNumber = EXTRAKEY_INTERFACE,
		.ReportINEndpoint = {
			.Address = EXTRAKEY_IN_EPADDR,
			.Size = EXTRAKEY_EPSIZE,
			.Banks = 1,
		},
		.PrevReportINBuffer = NULL,
		.PrevReportINBufferSize = MAX(sizeof(ExtrakeySystemReport),
					      sizeof(ExtrakeyConsumerReport))
	}
};

void
EVENT_USB_Device_Connect(void)
{
	expReset();
}
void
EVENT_USB_Device_Disconnect(void)
{
	expClear();
}
void
EVENT_USB_Device_ConfigurationChanged(void)
{
	HID_Device_ConfigureEndpoints(&keyboardHIDIface);
	HID_Device_ConfigureEndpoints(&genericHIDIface);
	HID_Device_ConfigureEndpoints(&nkroHIDIface);
	HID_Device_ConfigureEndpoints(&extrakeyHIDIface);
	USB_Device_EnableSOFEvents();
}
void
EVENT_USB_Device_Reset(void)
{
	/* hopefully OS does this reset this after it has booted, which should
	 * drop the keyboard back into NKRO mode
	 */
	keyboardHIDIface.State.UsingReportProtocol = true;
}
void
EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&keyboardHIDIface);
	HID_Device_ProcessControlRequest(&genericHIDIface);
	HID_Device_ProcessControlRequest(&nkroHIDIface);
	HID_Device_ProcessControlRequest(&extrakeyHIDIface);
}
void
EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&keyboardHIDIface);
	HID_Device_MillisecondElapsed(&genericHIDIface);
	HID_Device_MillisecondElapsed(&nkroHIDIface);
	HID_Device_MillisecondElapsed(&extrakeyHIDIface);

	expMSTick();
}

/*
 * true if standard keyboard interface is set to "report protocol"... hopefully
 */
static bool
usingNKROReport(void)
{
	return keyboardHIDIface.State.UsingReportProtocol;
}

/*
 * Send keyboard state to host
 */
bool
CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t *const hidIfInf,
				    uint8_t *const reportID,
				    const uint8_t reportType,
				    void *reportData,
				    uint16_t *const reportSize)
{
	if (hidIfInf == &keyboardHIDIface && updateKeyboardReport) {
		updateKeyboardReport = false;

		if (usingNKROReport())
			return false;

		kbdFillReport(reportData);
		*reportSize = sizeof(USB_KeyboardReport_Data_t);

		return true;
	} else if (hidIfInf == &genericHIDIface) {
		diagFillReport(reportData, usingNKROReport());
		*reportSize = GENERIC_REPORT_SIZE;

		return false;
	} else if (hidIfInf == &nkroHIDIface && updateNKROReport) {
		updateNKROReport = false;

		if (!usingNKROReport())
			return false;

		kbdFillNKROReport(reportData);
		*reportSize = sizeof(NKROReport);

		return true;
	} else if (hidIfInf == &extrakeyHIDIface) {
		static uint8_t currReportID = EXTRAKEY_REPORTID_SYSTEM;
		if (*reportID)
			currReportID = *reportID;
		else
			*reportID = currReportID;

		if (currReportID == EXTRAKEY_REPORTID_SYSTEM &&
		    updateSystemReport) {
			updateSystemReport = false;

			bool changed = kbdFillSystemReport(reportData);
			*reportSize = sizeof(ExtrakeySystemReport);

			currReportID = EXTRAKEY_REPORTID_CONSUMER;

			return changed;
		} else if (currReportID == EXTRAKEY_REPORTID_CONSUMER &&
			   updateConsumerReport) {
			updateConsumerReport = false;

			bool changed = kbdFillConsumerReport(reportData);
			*reportSize = sizeof(ExtrakeyConsumerReport);

			currReportID = EXTRAKEY_REPORTID_SYSTEM;

			return changed;
		}
	}

	return false;
}

/*
 *
 */
void
CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t *const hidIfInf,
				     const uint8_t  reportID,
				     const uint8_t  reportType,
				     const void    *reportData,
				     const uint16_t reportSize)
{
	if (hidIfInf == &keyboardHIDIface) {
		if (!usingNKROReport())
			kbdReceiveReport(reportData);
	} else if (hidIfInf == &genericHIDIface) {
		diagReceiveReport(reportData);
	} else if (hidIfInf == &nkroHIDIface) {
		if (usingNKROReport())
			kbdReceiveNKROReport(reportData);
	}
}

/*
 *
 */
void
initHardware(void)
{
	/* disable watchdog */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* clock full speed */
	clock_prescale_set(clock_div_1);

	/* Hardware Initialization */
	vrefInit();
	srInit();
	srClear();
	srEnable();
	USB_Init();
}

/*
 *
 */
int
main(void)
{
	initHardware();
	sei();

	kbdInit();
	scanInit();
	layersInit();
	macrosInit();
	expLoad();

	_delay_ms(5);

	scanEnable();

	/* try loading vref from eeprom; if set to 0xffff, run calibration */
	vrefLoad();
	if (vrefGet() == 0xffff)
		kbdCalibrate();

	for (;;) {
		/* try to keep updates synchronous; we don't want to update
		 * bitmap if we haven't sent the last report yet.
		 */
		if (( usingNKROReport() && !updateNKROReport) ||
		    (!usingNKROReport() && !updateKeyboardReport)) {
			bool scanChanged = kbdUpdateSCBmp();
			bool expChanged  = expProcessScan(scanChanged);
			if (scanChanged || expChanged) {
				updateKeyboardReport = true;
				updateNKROReport     = true;
				updateSystemReport   = true;
				updateConsumerReport = true;
			}
			layersProcessScan();
		}

		if (USB_DeviceState == DEVICE_STATE_Unattached ||
		    USB_DeviceState == DEVICE_STATE_Suspended)
			if (USB_Device_RemoteWakeupEnabled && kbdWantsWakeup())
				USB_Device_SendRemoteWakeup();

		HID_Device_USBTask(&keyboardHIDIface);
		HID_Device_USBTask(&genericHIDIface);
		HID_Device_USBTask(&nkroHIDIface);
		HID_Device_USBTask(&extrakeyHIDIface);
		USB_USBTask();
	}
}
