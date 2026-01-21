QT += core gui widgets

CONFIG += c++14

TARGET = sample_qt
TEMPLATE = app

HOME_PATH        = ../../..
MSP_OUT_PATH     = $$HOME_PATH/msp/out
THIRD_PARTY_PATH = $$HOME_PATH/third-party
DRM_PATH         = $$THIRD_PARTY_PATH/drm/lib

DEPENDPATH  += $$MSP_OUT_PATH/lib
INCLUDEPATH += $$MSP_OUT_PATH/include

QMAKE_CFLAGS = `pkg-config --cflags -Wl,-rpath-link=$$(LIB_PATH)`

QMAKE_CXXFLAGS = $$QMAKE_CFLAGS

LIBS += -L$$MSP_OUT_PATH/lib
LIBS += -lax_sys
LIBS += -lax_ivps
LIBS += -lax_vo
LIBS += -L$$THIRD_PARTY_PATH/drm/lib -ldrm

UI_SRC_DIR = ui

CONFIG(debug, debug|release) {
    DESTDIR = build/debug
}
CONFIG(release, debug|release) {
    DESTDIR = build/release
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.u
OBJECTS_DIR = $$DESTDIR/.obj

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    $$UI_SRC_DIR/main.cpp \
    $$UI_SRC_DIR/rasterwindow.cpp

HEADERS += \
    $$UI_SRC_DIR/rasterwindow.h



