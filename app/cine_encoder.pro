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
QT += svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11
#CONFIG += console
CODECFORSRC = UTF-8

# GENERAL
TARGET = cine_encoder
TEMPLATE = app
VERSION = 3.5.4

# DIRS
CONFIG -= debug_and_release debug_and_release_target
DESTDIR =     ../builddir
OBJECTS_DIR = ../builddir/obj
MOC_DIR =     ../builddir/moc
UI_DIR =      ../builddir/ui
RCC_DIR =     ../builddir/rcc

# DEFINES
#DEFINES += QT_DEPRECATED_X

# FILES
SOURCES += \
    basedialog.cpp \
    encoder.cpp \
    helper.cpp \
    main.cpp \
    mainwindow.cpp \
    message.cpp \
    notification.cpp \
    preset.cpp \
    progress.cpp \
    settings.cpp

HEADERS += \
    basedialog.h \
    constants.h \
    encoder.h \
    helper.h \
    mainwindow.h \
    message.h \
    notification.h \
    preset.h \
    progress.h \
    settings.h \
    tables.h

FORMS += \
    mainwindow.ui \
    message.ui \
    notification.ui \
    preset.ui \
    progress.ui \
    settings.ui

unix:!macx {
    SOURCES += \
        platform_unix/basewindow.cpp \
        platform_unix/xutil.cpp

    HEADERS += \
        platform_unix/basewindow.h \
        platform_unix/xutil.h
}

win32 {
    RC_FILE = icon.rc
    SOURCES += \
        platform_win/basewindow.cpp

    HEADERS += \
        platform_win/basewindow.h
}

TRANSLATIONS += $$PWD/resources/translation/translation_de.ts \
                $$PWD/resources/translation/translation_ru.ts \
                $$PWD/resources/translation/translation_zh.ts \
                $$PWD/resources/translation/translation_nl.ts

RESOURCES += files.qrc

# LIBS
unix:!macx {
    QT += x11extras dbus
    CONFIG += link_pkgconfig
    PKGCONFIG += xext
    LIBS += -lX11 \
            -lmediainfo
}

win32 {
    LIBS += -luser32 \
            -ldwmapi
}

# INSTALLS
qnx: target.path = /tmp/bin
else: unix:!android {
    target.path = /usr/bin
}
!isEmpty(target.path): INSTALLS += target

# BIN     = $(DESTDIR)/usr/bin
# ICONS   = $(DESTDIR)/usr/share/icons/hicolor/64x64/apps
