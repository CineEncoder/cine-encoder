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
CODECFORSRC = UTF-8

# GENERAL
TARGET = cine_encoder
TEMPLATE = app
VERSION = 3.5.4

# DIRS
CONFIG -= debug_and_release debug_and_release_target
DESTDIR =     $$PWD/../builddir
OBJECTS_DIR = $$PWD/../builddir/obj
MOC_DIR =     $$PWD/../builddir/moc
UI_DIR =      $$PWD/../builddir/ui
RCC_DIR =     $$PWD/../builddir/rcc

# DEFINES
#DEFINES += QT_DEPRECATED_X

# FILES
SOURCES += \
    fileiconprovider.cpp \
    popupmessage.cpp \
    report.cpp \
    widgets/qstreamview.cpp \
    widgets/qanimatedsvg.cpp \
    widgets/qdoublebutton.cpp \
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
    fileiconprovider.h \
    popupmessage.h \
    report.h \
    widgets/qstreamview.h \
    widgets/qanimatedsvg.h \
    widgets/qdoublebutton.h \
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
    popupmessage.ui \
    preset.ui \
    progress.ui \
    report.ui \
    settings.ui


contains(DEFINES, WM_CUSTOM) {
    SOURCES += \
        basewindow_cwm.cpp
    HEADERS += \
        basewindow_cwm.h
} else {
    unix:!macx {
        SOURCES += \
            platform_unix/basewindow.cpp \
            platform_unix/xutil.cpp

        HEADERS += \
            platform_unix/basewindow.h \
            platform_unix/xutil.h
    }
    win32 {
        SOURCES += \
            platform_win/basewindow.cpp

        HEADERS += \
            platform_win/basewindow.h
    }
}

win32 {
    RC_FILE = icon.rc
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
