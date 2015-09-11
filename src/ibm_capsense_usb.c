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
#include "sr.h"
#include "vref.h"

static uint8_t prevKeyboardHIDReportBuf[sizeof(USB_KeyboardReport_Data_t)];
static uint8_t prevGenericHIDReportBuf[GENERIC_REPORT_SIZE];
static uint8_t prevNKROHIDReportBuf[sizeof(NKROReport)];

USB_ClassInfo_HID_Device_t keyboardHIDIface = {
	.Config = {
		.InterfaceNumber = KEYBOARD_INTERFACE,
		.ReportINEndpoint = {
			.Address = KEYBOARD_IN_EPADDR,
			.Size = KBD_EPSIZE,
			.Banks = 1,
		},
		.PrevReportINBuffer = prevKeyboardHIDReportBuf,
		.PrevReportINBufferSize = sizeof(prevKeyboardHIDReportBuf),
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
		.PrevReportINBuffer = prevGenericHIDReportBuf,
		.PrevReportINBufferSize = sizeof(prevGenericHIDReportBuf),
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
		.PrevReportINBuffer = prevNKROHIDReportBuf,
		.PrevReportINBufferSize = sizeof(prevNKROHIDReportBuf),
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
	USB_Device_EnableSOFEvents();
}
void
EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&keyboardHIDIface);
	HID_Device_ProcessControlRequest(&genericHIDIface);
	HID_Device_ProcessControlRequest(&nkroHIDIface);
}
void
EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&keyboardHIDIface);
	HID_Device_MillisecondElapsed(&genericHIDIface);
	HID_Device_MillisecondElapsed(&nkroHIDIface);

	expMSTick();
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
	if (hidIfInf == &keyboardHIDIface) {
		kbdFillReport(reportData);

		*reportSize = sizeof(USB_KeyboardReport_Data_t);
		return false;
	} else if (hidIfInf == &genericHIDIface) {
		diagFillReport(reportData);

		*reportSize = GENERIC_REPORT_SIZE;
		return false;
	} else {
		kbdFillNKROReport(reportData,
				  hidIfInf->Config.PrevReportINBuffer);
		*reportSize = sizeof(NKROReport);
		return false;
	}
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
	if (hidIfInf == &keyboardHIDIface)
		kbdReceiveReport(reportData);
	else if (hidIfInf == &genericHIDIface)
		diagReceiveReport(reportData);
	else
		kbdReceiveNKROReport(reportData);
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
	expInit();
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
	kbdLoadColSkips();

	layersLoad();
	expLoad();

	_delay_ms(5);

	/* try loading Vref from eeprom; if set to 0xffff, run calibration */
	vrefLoad();
	if (vrefGet() == 0xffff)
		kbdCalibrate();

	for (;;) {
		kbdScan();

		if (USB_DeviceState == DEVICE_STATE_Unattached ||
		    USB_DeviceState == DEVICE_STATE_Suspended)
			if (USB_Device_RemoteWakeupEnabled && kbdWantsWakeup())
				USB_Device_SendRemoteWakeup();

		HID_Device_USBTask(&keyboardHIDIface);
		HID_Device_USBTask(&genericHIDIface);
		HID_Device_USBTask(&nkroHIDIface);
		USB_USBTask();
	}
}
