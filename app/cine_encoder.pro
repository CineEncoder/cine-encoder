#***********************************************************************
#
#                          C I N E   E N C O D E R
#                                JULY, 2020
#                            COPYRIGHT (C) 2020
#
# FILE: cine_encoder.pro
# LICENSE: GNU General Public License v3.0
#
#***********************************************************************

# QT
QT += core
QT += gui
QT += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11
CODECFORSRC = UTF-8
#CONFIG += console
# GENERAL
TARGET = cine_encoder
TEMPLATE = app
VERSION = 3.5.3

# DIRS
CONFIG -= debug_and_release debug_and_release_target
DESTDIR = ../builddir
OBJECTS_DIR = ../builddir/obj
MOC_DIR = ../builddir/moc
UI_DIR = ../builddir/ui
RCC_DIR = ../builddir/rcc

# DEFINES
DEFINES += QT_DEPRECATED_X

# FILES
SOURCES += \
    about.cpp \
    dialog.cpp \
    donate.cpp \
    encoder.cpp \
    framelesswindow.cpp \
    main.cpp \
    mainwindow.cpp \
    openingfiles.cpp \
    preset.cpp \
    settings.cpp \
    taskcomplete.cpp

HEADERS += \
    about.h \
    constants.h \
    dialog.h \
    donate.h \
    encoder.h \
    framelesswindow.h \
    mainwindow.h \
    openingfiles.h \
    preset.h \
    settings.h \
    taskcomplete.h

FORMS += \
    about.ui \
    dialog.ui \
    donate.ui \
    mainwindow.ui \
    openingfiles.ui \
    preset.ui \
    settings.ui \
    taskcomplete.ui

RESOURCES += \
    files.qrc

win32 {
    RC_FILE = icon.rc
}

TRANSLATIONS += translation_de.ts \
                translation_ru.ts \
                translation_zh.ts

# LIBS
unix:!macx {
    LIBS += -lmediainfo
}

# INSTALLS
qnx: target.path = /tmp/bin
else: unix:!android {
    target.path = /usr/bin
}
!isEmpty(target.path): INSTALLS += target

# BIN     = $(DESTDIR)/usr/bin
# ICONS   = $(DESTDIR)/usr/share/icons/hicolor/64x64/apps
