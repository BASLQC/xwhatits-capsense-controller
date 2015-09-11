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
#include "descriptors.h"

/* marked as padding so OS ignores it and uses NKRO. BIOS will still use it. */
const USB_Descriptor_HIDReport_Datatype_t PROGMEM KeyboardReport[] = {
	HID_RI_USAGE_PAGE(8, 0x01), 
	HID_RI_USAGE(8, 0x06), 
	HID_RI_COLLECTION(8, 0x01), 
		HID_RI_REPORT_SIZE(8, 0x01),
		HID_RI_REPORT_COUNT(8, 0x08),
		HID_RI_INPUT(8, HID_IOF_CONSTANT),

		HID_RI_REPORT_COUNT(8, 0x01),
		HID_RI_REPORT_SIZE(8, 0x08),
		HID_RI_INPUT(8, HID_IOF_CONSTANT),

		HID_RI_REPORT_COUNT(8, 0x05),
		HID_RI_REPORT_SIZE(8, 0x01),
		HID_RI_OUTPUT(8, HID_IOF_CONSTANT),

		HID_RI_REPORT_COUNT(8, 0x01),
		HID_RI_REPORT_SIZE(8, 0x03),
		HID_RI_OUTPUT(8, HID_IOF_CONSTANT),

		HID_RI_REPORT_COUNT(8, 6),
		HID_RI_REPORT_SIZE(8, 0x08),
		HID_RI_INPUT(8, HID_IOF_CONSTANT),
	HID_RI_END_COLLECTION(0)
};

const USB_Descriptor_HIDReport_Datatype_t PROGMEM NKROReport[] = {
	HID_RI_USAGE_PAGE(8, 0x01),
	HID_RI_USAGE(8, 0x06),
	HID_RI_COLLECTION(8, 0x01),
		HID_RI_USAGE_PAGE(8, 0x08),      // LED page
		HID_RI_USAGE_MINIMUM(8, 0x01),   // num lock
		HID_RI_USAGE_MAXIMUM(8, 0x03),   // scroll lock
		HID_RI_LOGICAL_MINIMUM(8, 0x00),
		HID_RI_LOGICAL_MAXIMUM(8, 0x01),
		HID_RI_REPORT_COUNT(8, 0x03),
		HID_RI_REPORT_SIZE(8, 0x01),
		HID_RI_OUTPUT(8,
			      HID_IOF_DATA |
			      HID_IOF_VARIABLE |
			      HID_IOF_ABSOLUTE),

                HID_RI_REPORT_COUNT(8, 0x01),
                HID_RI_REPORT_SIZE(8, 0x05),
                HID_RI_OUTPUT(8, HID_IOF_CONSTANT),

		HID_RI_USAGE_PAGE(8, 0x07),      // key codes page
		HID_RI_USAGE_MINIMUM(8, 0xe0),   // left ctrl
		HID_RI_USAGE_MAXIMUM(8, 0xe7),   // right GUI
		HID_RI_LOGICAL_MINIMUM(8, 0x00),
		HID_RI_LOGICAL_MAXIMUM(8, 0x01),
		HID_RI_REPORT_COUNT(8, 0x08),
		HID_RI_REPORT_SIZE(8, 0x01),
		HID_RI_INPUT(8,
			     HID_IOF_DATA |
			     HID_IOF_VARIABLE |
			     HID_IOF_ABSOLUTE),

		HID_RI_USAGE_PAGE(8, 0x07),      // key codes page
		HID_RI_USAGE_MINIMUM(8, 0x00),   // Reserved
		HID_RI_USAGE_MAXIMUM(8, 0x31),   // Backslash/Bar
		HID_RI_LOGICAL_MINIMUM(8, 0x00),
		HID_RI_LOGICAL_MAXIMUM(8, 0x01),
		HID_RI_REPORT_COUNT(8, 0x32),
		HID_RI_REPORT_SIZE(8, 0x01),
		HID_RI_INPUT(8,
			     HID_IOF_DATA |
			     HID_IOF_VARIABLE |
			     HID_IOF_ABSOLUTE),

		HID_RI_REPORT_COUNT(8, 0x01), // skip Non-US # and ~, workaround
		HID_RI_REPORT_SIZE(8, 0x01),  // for linux bug with backslashes
		HID_RI_INPUT(8, HID_IOF_CONSTANT),

		HID_RI_USAGE_PAGE(8, 0x07),      // key codes page
		HID_RI_USAGE_MINIMUM(8, 0x33),   // semicolon/colon
		HID_RI_USAGE_MAXIMUM(8, 0x9b),   // cancel
		HID_RI_LOGICAL_MINIMUM(8, 0x00),
		HID_RI_LOGICAL_MAXIMUM(8, 0x01),
		HID_RI_REPORT_COUNT(8, 0x69),
		HID_RI_REPORT_SIZE(8, 0x01),
		HID_RI_INPUT(8,
			     HID_IOF_DATA |
			     HID_IOF_VARIABLE |
			     HID_IOF_ABSOLUTE),

		HID_RI_REPORT_COUNT(8, 0x01), // skip clear, workaround for
		HID_RI_REPORT_SIZE(8, 0x01),  // linux bug (similar to above)
		HID_RI_INPUT(8, HID_IOF_CONSTANT),

		HID_RI_USAGE_PAGE(8, 0x07),      // key codes page
		HID_RI_USAGE_MINIMUM(8, 0x9d),   // prior
		HID_RI_USAGE_MAXIMUM(8, 0xa4),   // exsel
		HID_RI_LOGICAL_MINIMUM(8, 0x00),
		HID_RI_LOGICAL_MAXIMUM(8, 0x01),
		HID_RI_REPORT_COUNT(8, 0x08),
		HID_RI_REPORT_SIZE(8, 0x01),
		HID_RI_INPUT(8,
			     HID_IOF_DATA |
			     HID_IOF_VARIABLE |
			     HID_IOF_ABSOLUTE),

		HID_RI_USAGE_PAGE(8, 0x01),      // generic desktop page
		HID_RI_USAGE_MINIMUM(8, 0x81),   // system power down
		HID_RI_USAGE_MAXIMUM(8, 0x83),   // system wake up
		HID_RI_LOGICAL_MINIMUM(8, 0x00),
		HID_RI_LOGICAL_MAXIMUM(8, 0x01),
		HID_RI_REPORT_COUNT(8, 0x03),
		HID_RI_REPORT_SIZE(8, 0x01),
		HID_RI_INPUT(8,
			     HID_IOF_DATA |
			     HID_IOF_VARIABLE |
			     HID_IOF_ABSOLUTE),

		HID_RI_USAGE_PAGE(8, 0x0c),      // consumer page
		HID_RI_USAGE_MINIMUM(8, 0xb0),   // play
		HID_RI_USAGE_MAXIMUM(8, 0xb8),   // eject
		HID_RI_LOGICAL_MINIMUM(8, 0x00),
		HID_RI_LOGICAL_MAXIMUM(8, 0x01),
		HID_RI_REPORT_COUNT(8, 0x09),
		HID_RI_REPORT_SIZE(8, 0x01),
		HID_RI_INPUT(8,
			     HID_IOF_DATA |
			     HID_IOF_VARIABLE |
			     HID_IOF_ABSOLUTE),

		HID_RI_USAGE_PAGE(8, 0x0c), // consumer page
		HID_RI_USAGE(8, 0xcd),      // play/pause
		HID_RI_USAGE(8, 0xe2),      // mute
		HID_RI_USAGE(8, 0xe9),      // volume++
		HID_RI_USAGE(8, 0xea),      // volume--
		HID_RI_USAGE(16, 0x18a),    // mail
		HID_RI_USAGE(16, 0x192),    // calculator
		HID_RI_USAGE(16, 0x194),    // file browser
		HID_RI_USAGE(16, 0x221),    // web search
		HID_RI_USAGE(16, 0x223),    // web home
		HID_RI_USAGE(16, 0x224),    // web back
		HID_RI_USAGE(16, 0x225),    // web forward
		HID_RI_USAGE(16, 0x227),    // web refresh
		HID_RI_USAGE(16, 0x22a),    // web bookmarks
		HID_RI_LOGICAL_MINIMUM(8, 0x00),
		HID_RI_LOGICAL_MAXIMUM(8, 0x01),
		HID_RI_REPORT_COUNT(8, 0x0d),
		HID_RI_REPORT_SIZE(8, 0x01),
		HID_RI_INPUT(8,
			     HID_IOF_DATA |
			     HID_IOF_VARIABLE |
			     HID_IOF_ABSOLUTE),

		HID_RI_REPORT_COUNT(8, 0x01),
		HID_RI_REPORT_SIZE(8, 0x02),
		HID_RI_INPUT(8, HID_IOF_CONSTANT),
	HID_RI_END_COLLECTION(0)
};

const USB_Descriptor_HIDReport_Datatype_t PROGMEM GenericReport[] = {
	HID_DESCRIPTOR_VENDOR(0x00, 0x01, 0x02, 0x03, GENERIC_REPORT_SIZE)
};

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
	.Header = {
		.Size = sizeof(USB_Descriptor_Device_t),
		.Type = DTYPE_Device
	},

	.USBSpecification       = VERSION_BCD(01.10),
	.Class                  = USB_CSCP_NoDeviceClass,
	.SubClass               = USB_CSCP_NoDeviceSubclass,
	.Protocol               = USB_CSCP_NoDeviceProtocol,

	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

	.VendorID               = 0x0481,
	.ProductID              = 0x0002,
	.ReleaseNumber          = VERSION_BCD(00.01),

	.ManufacturerStrIndex   = 0x01,
	.ProductStrIndex        = 0x02,
	.SerialNumStrIndex      = NO_DESCRIPTOR,

	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor = {
	.Config = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Configuration_Header_t),
			.Type = DTYPE_Configuration
		},

		.TotalConfigurationSize =
		   sizeof(USB_Descriptor_Configuration_t),
		.TotalInterfaces        = 3,

		.ConfigurationNumber    = 1,
		.ConfigurationStrIndex  = NO_DESCRIPTOR,

		.ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED |
					   USB_CONFIG_ATTR_REMOTEWAKEUP),

		.MaxPowerConsumption    = USB_CONFIG_POWER_MA(USB_CURRENT)
	},

	.KeyboardInterface = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Interface_t),
			.Type = DTYPE_Interface
		},

		.InterfaceNumber   = KEYBOARD_INTERFACE,
		.AlternateSetting  = 0x00,

		.TotalEndpoints    = 1,

		.Class             = HID_CSCP_HIDClass,
		.SubClass          = HID_CSCP_BootSubclass,
		.Protocol          = HID_CSCP_KeyboardBootProtocol,

		.InterfaceStrIndex = NO_DESCRIPTOR
	},

	.KeyboardHID = {
		.Header = {
			.Size = sizeof(USB_HID_Descriptor_HID_t),
			.Type = HID_DTYPE_HID
		},

		.HIDSpec                = VERSION_BCD(01.11),
		.CountryCode            = 0x00,
		.TotalReportDescriptors = 1,
		.HIDReportType          = HID_DTYPE_Report,
		.HIDReportLength        = sizeof(KeyboardReport)
	},

	.KeyboardInEndpoint = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Endpoint_t),
			.Type = DTYPE_Endpoint
		},

		.EndpointAddress   = KEYBOARD_IN_EPADDR,
		.Attributes        = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC |
				      ENDPOINT_USAGE_DATA),
		.EndpointSize      = KBD_EPSIZE,
		.PollingIntervalMS = 0x0a
	},

	.GenericInterface = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Interface_t),
			.Type = DTYPE_Interface
		},

		.InterfaceNumber   = GENERIC_INTERFACE,
		.AlternateSetting  = 0x00,

		.TotalEndpoints    = 1,

		.Class             = HID_CSCP_HIDClass,
		.SubClass          = HID_CSCP_NonBootSubclass,
		.Protocol          = HID_CSCP_NonBootProtocol,

		.InterfaceStrIndex = NO_DESCRIPTOR
	},

	.GenericHID = {
		.Header = {
			.Size = sizeof(USB_HID_Descriptor_HID_t),
			.Type = HID_DTYPE_HID
		},

		.HIDSpec                = VERSION_BCD(01.11),
		.CountryCode            = 0x00,
		.TotalReportDescriptors = 1,
		.HIDReportType          = HID_DTYPE_Report,
		.HIDReportLength        = sizeof(GenericReport)
	},

	.GenericInEndpoint = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Endpoint_t),
			.Type = DTYPE_Endpoint
		},

		.EndpointAddress   = GENERIC_IN_EPADDR,
		.Attributes        = (EP_TYPE_INTERRUPT |
				      ENDPOINT_ATTR_NO_SYNC |
				      ENDPOINT_USAGE_DATA),
		.EndpointSize      = GENERIC_EPSIZE,
		.PollingIntervalMS = 0x0a
	},

	.NKROInterface = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Interface_t),
			.Type = DTYPE_Interface
		},

		.InterfaceNumber   = NKRO_INTERFACE,
		.AlternateSetting  = 0x00,

		.TotalEndpoints    = 1,

		.Class             = HID_CSCP_HIDClass,
		.SubClass          = HID_CSCP_NonBootSubclass,
		.Protocol          = HID_CSCP_NonBootProtocol,

		.InterfaceStrIndex = NO_DESCRIPTOR
	},

	.NKROHID = {
		.Header = {
			.Size = sizeof(USB_HID_Descriptor_HID_t),
			.Type = HID_DTYPE_HID
		},

		.HIDSpec                = VERSION_BCD(01.11),
		.CountryCode            = 0x00,
		.TotalReportDescriptors = 1,
		.HIDReportType          = HID_DTYPE_Report,
		.HIDReportLength        = sizeof(NKROReport)
	},

	.NKROInEndpoint = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Endpoint_t),
			.Type = DTYPE_Endpoint
		},

		.EndpointAddress   = NKRO_IN_EPADDR,
		.Attributes        = (EP_TYPE_INTERRUPT |
				      ENDPOINT_ATTR_NO_SYNC |
				      ENDPOINT_USAGE_DATA),
		.EndpointSize      = NKRO_EPSIZE,
		.PollingIntervalMS = 0x0a
	},
};

const USB_Descriptor_String_t PROGMEM LanguageString = {
	.Header = {
		.Size = USB_STRING_LEN(1),
		.Type = DTYPE_String
	},

	.UnicodeString = {LANGUAGE_ID_ENG}
};

const USB_Descriptor_String_t PROGMEM ManufacturerString = {
	.Header = {
		.Size = USB_STRING_LEN(16),
		.Type = DTYPE_String
	},

	.UnicodeString = L"Tom Wong-Cornall"
};

const USB_Descriptor_String_t PROGMEM ProductString = {
	.Header = {
		.Size = USB_STRING_LEN(16),
		.Type = DTYPE_String
	},

	.UnicodeString = L"ibm-capsense-usb"
};

uint16_t
CALLBACK_USB_GetDescriptor(const uint16_t wValue,
			   const uint8_t wIndex,
			   const void **const DescriptorAddress)
{
	const uint8_t DescriptorType   = (wValue >> 8);
	const uint8_t DescriptorNumber = (wValue &  0xFF);

	const void *Address = NULL;
	uint16_t Size = NO_DESCRIPTOR;

	switch (DescriptorType) {
	case DTYPE_Device:
		Address = &DeviceDescriptor;
		Size = sizeof(USB_Descriptor_Device_t);
		break;
	case DTYPE_Configuration:
		Address = &ConfigurationDescriptor;
		Size = sizeof(USB_Descriptor_Configuration_t);
		break;
	case DTYPE_String:
		switch (DescriptorNumber) {
		case 0x00:
			Address = &LanguageString;
			Size = pgm_read_byte(&LanguageString.Header.Size);
			break;
		case 0x01:
			Address = &ManufacturerString;
			Size = pgm_read_byte(&ManufacturerString.Header.Size);
			break;
		case 0x02:
			Address = &ProductString;
			Size = pgm_read_byte(&ProductString.Header.Size);
			break;
		}
		break;
	case HID_DTYPE_HID:
		switch (wIndex) {
		case KEYBOARD_INTERFACE:
			Address = &ConfigurationDescriptor.KeyboardHID;
			Size = sizeof(USB_HID_Descriptor_HID_t);
			break;
		case GENERIC_INTERFACE:
			Address = &ConfigurationDescriptor.GenericHID;
			Size = sizeof(USB_HID_Descriptor_HID_t);
			break;
		case NKRO_INTERFACE:
			Address = &ConfigurationDescriptor.NKROHID;
			Size = sizeof(USB_HID_Descriptor_HID_t);
			break;
		}
		break;
	case HID_DTYPE_Report:
		switch (wIndex) {
		case KEYBOARD_INTERFACE:
			Address = &KeyboardReport;
			Size = sizeof(KeyboardReport);
			break;
		case GENERIC_INTERFACE:
			Address = &GenericReport;
			Size = sizeof(GenericReport);
			break;
		case NKRO_INTERFACE:
			Address = &NKROReport;
			Size = sizeof(NKROReport);
			break;
		}
		break;
	}

	*DescriptorAddress = Address;
	return Size;
}

