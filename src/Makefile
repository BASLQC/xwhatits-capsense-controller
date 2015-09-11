#
#             LUFA Library
#     Copyright (C) Dean Camera, 2013.
#
#  dean [at] fourwalledcubicle [dot] com
#           www.lufa-lib.org
#
# --------------------------------------
#         LUFA Project Makefile.
# --------------------------------------

# The board type and revision must match the controller

#BOARD_TYPE    = BEAMSPRING
#BOARD_TYPE    = BEAMSPRING_DISPLAYWRITER
BOARD_TYPE    = MODEL_F
BOARD_REV     = 1

# USB current would normally be 100, unless used with solenoid driver---use 500
USB_CURRENT   = 100
#USB_CURRENT   = 500

# Should normally leave set to KEY_POLARITY_AUTO, in which case will figure it
# out from board type. Overrides useful for things like weirdo 3178-lookalikes
# with 23x4 matrixes, using beamspring controllers.
KEY_POLARITY  = KEY_POLARITY_AUTO        # figure out from board type
#KEY_POLARITY  = KEY_POLARITY_ACTIVE_HIGH # Model Fs, Amkey etc.
#KEY_POLARITY  = KEY_POLARITY_ACTIVE_LOW  # Beamsprings

VERSION      = 0.9.0

MCU          = atmega32u2
ARCH         = AVR8
BOARD        = NONE
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = ibm_capsense_usb
SRC          = $(TARGET).c \
	       dac101s101.c \
	       descriptors.c \
	       diag.c \
	       exp.c \
	       kbd.c \
	       layers.c \
	       macros.c \
	       mcp4726.c \
	       scan.c \
	       spi.c \
	       sr.c \
	       twi.c \
	       $(LUFA_SRC_USB) \
	       $(LUFA_SRC_USBCLASS)
LUFA_PATH    = lib/LUFA-130303/LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER \
	       -Icfg/ \
	       -Wall \
	       -flto \
	       -DVERSION=\"$(VERSION)\" \
	       -D$(BOARD_TYPE) \
	       -DBOARD_REV=$(BOARD_REV) \
	       -DUSB_CURRENT=$(USB_CURRENT) \
	       -D$(KEY_POLARITY)
LD_FLAGS     =

# Default target
all:

# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
include $(LUFA_PATH)/Build/lufa_doxygen.mk
include $(LUFA_PATH)/Build/lufa_dfu.mk
include $(LUFA_PATH)/Build/lufa_hid.mk
include $(LUFA_PATH)/Build/lufa_avrdude.mk
include $(LUFA_PATH)/Build/lufa_atprogram.mk
