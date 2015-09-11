VERSION = 0.7.1
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
    CONFIG += debug
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
        src/DiagInterface.h \
        src/Frontend.h \
        src/ImportExport.h \
        src/Key.h \
        src/KeyMon.h \
        src/LayerConditionWatcher.h \
        src/NonFocused.h \
        src/Scancodes.h

SOURCES += \
        src/DiagInterface.cpp \
        src/Frontend.cpp \
        src/ImportExport.cpp \
        src/Key.cpp \
        src/KeyMon.cpp \
        src/LayerConditionWatcher.cpp \
        src/NonFocused.cpp \
        src/Scancodes.cpp \
        src/ibm_capsense_usb_util.cpp

macx {
    SOURCES += ../../../hidapi-0.7.0/mac/hid.c
}
