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
USB_CURRENT   = 100

VERSION      = 0.7.0

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
	       mcp4726.c \
	       spi.c \
	       sr.c \
	       twi.c \
	       $(LUFA_SRC_USB) \
	       $(LUFA_SRC_USBCLASS)
LUFA_PATH    = lib/LUFA-130303/LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER \
	       -Icfg/ \
	       -Wall\
	       -DVERSION=\"$(VERSION)\" \
	       -D$(BOARD_TYPE) \
	       -DBOARD_REV=$(BOARD_REV) \
	       -DUSB_CURRENT=$(USB_CURRENT)
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
