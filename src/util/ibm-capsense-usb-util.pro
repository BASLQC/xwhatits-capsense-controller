VERSION = 0.9.0
VERSTR = '\\"$${VERSION}\\"'
DEFINES += VER=\"$${VERSTR}\"

TEMPLATE = app

TARGET = ibm_capsense_usb_util
DESTDIR = src

OBJECTS_DIR = .build
MOC_DIR = .build
RCC_DIR = .build

QT += \
        core \
        gui \
        widgets

INCLUDEPATH += \
        ../ \
        src

QMAKE_CXXFLAGS += -std=c++0x -g

unix:!macx {
    LIBS += -lhidapi-libusb
    INCLUDEPATH += \
        /usr/include \
        /usr/include/hidapi
    CONFIG += debug silent
}
win32 { 
    LIBS += -Lc:/lib -lhidapi-0
    INCLUDEPATH += c:/include \
                   c:/include/hidapi
    CONFIG += release
}
macx {
    INCLUDEPATH += ../../../hidapi-0.7.0/hidapi
    LIBS += -framework IOKit -framework CoreFoundation
    CONFIG += c++11 debug
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7
    QMAKE_CXXFLAGS += -stdlib=libc++
}

HEADERS += \
        src/ColSkips.h \
        src/DiagInterface.h \
        src/ExpansionHeader.h \
        src/Frontend.h \
        src/ImportExport.h \
        src/Key.h \
        src/KeyMon.h \
        src/LayerConditions.h \
        src/Macro.h \
        src/Macros.h \
        src/NonFocused.h \
        src/PaddedBox.h \
        src/Scancodes.h \
        src/StatusBar.h \
        src/VoltageThreshold.h

SOURCES += \
        src/ColSkips.cpp \
        src/DiagInterface.cpp \
        src/ExpansionHeader.cpp \
        src/Frontend.cpp \
        src/ImportExport.cpp \
        src/Key.cpp \
        src/KeyMon.cpp \
        src/LayerConditions.cpp \
        src/Macro.cpp \
        src/Macros.cpp \
        src/NonFocused.cpp \
        src/PaddedBox.cpp \
        src/Scancodes.cpp \
        src/StatusBar.cpp \
        src/VoltageThreshold.cpp \
        src/ibm_capsense_usb_util.cpp

macx {
    SOURCES += ../../../hidapi-0.7.0/mac/hid.c
}
