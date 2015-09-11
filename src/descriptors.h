/******************************************************************************
  Copyright 2013  Dean Camera (dean [at] fourwalledcubicle [dot] com)
  Copyright 2010  Denver Gingerich (denver [at] ossguy [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
 ******************************************************************************/
#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

#include <LUFA/Drivers/USB/USB.h>
#include <avr/pgmspace.h>

typedef struct {
	USB_Descriptor_Configuration_Header_t Config;

	/* keyboard interface */
	USB_Descriptor_Interface_t KeyboardInterface;
	USB_HID_Descriptor_HID_t KeyboardHID;
	USB_Descriptor_Endpoint_t KeyboardInEndpoint;

	/* generic HID debug interface */
	USB_Descriptor_Interface_t GenericInterface;
	USB_HID_Descriptor_HID_t GenericHID;
	USB_Descriptor_Endpoint_t GenericInEndpoint;

	/* nkro interface */
	USB_Descriptor_Interface_t NKROInterface;
	USB_HID_Descriptor_HID_t NKROHID;
	USB_Descriptor_Endpoint_t NKROInEndpoint;
} USB_Descriptor_Configuration_t;

#define KEYBOARD_IN_EPADDR (ENDPOINT_DIR_IN | 1)
#define NKRO_IN_EPADDR     (ENDPOINT_DIR_IN | 2)
#define GENERIC_IN_EPADDR  (ENDPOINT_DIR_IN | 3)

#define KEYBOARD_INTERFACE 0x00
#define GENERIC_INTERFACE  0x01
#define NKRO_INTERFACE     0x02

#define KBD_EPSIZE      8
#define NKRO_EPSIZE    25
#define GENERIC_EPSIZE  8

#define GENERIC_REPORT_SIZE 8

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
				    const uint8_t wIndex,
				    const void **const DescriptorAddress)
	ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif
