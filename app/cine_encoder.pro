QT += core
QT += gui
QT += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

unix:!macx {
    LIBS += -lmediainfo
}

# Default rules for deployment.

TARGET = cine_encoder
TEMPLATE = app

#OBJECTS_DIR = ../builddir/obj
#MOC_DIR = ../builddir/moc
#UI_DIR = ../builddir/ui
#RCC_DIR = ../builddir/rcc

#RES_DIR = $${PWD}/unknownsys_build
#win32: RES_DIR = $${PWD}/builddir
#unix:  RES_DIR = $${PWD}/builddir
#macx: RES_DIR = $${PWD}/builddir

#DESTDIR = $${RES_DIR}
#OBJECTS_DIR = $${RES_DIR}/obj
#MOC_DIR = $${RES_DIR}/moc
#UI_DIR = $${RES_DIR}/ui
#RCC_DIR = $${RES_DIR}/rcc
#OUT_PWD = $${RES_DIR}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about.cpp \
    dialog.cpp \
    donate.cpp \
    main.cpp \
    mainwindow.cpp \
    preset.cpp \
    selectpreset.cpp \
    settings.cpp \
    taskcomplete.cpp

HEADERS += \
    about.h \
    dialog.h \
    donate.h \
    mainwindow.h \
    preset.h \
    selectpreset.h \
    settings.h \
    taskcomplete.h

FORMS += \
    about.ui \
    dialog.ui \
    donate.ui \
    mainwindow.ui \
    preset.ui \
    selectpreset.ui \
    settings.ui \
    taskcomplete.ui

RESOURCES += \
    files.qrc

win32 {
    RC_FILE = icon.rc
}

qnx: target.path = /tmp/bin
else: unix:!android {
    target.path = /usr/bin
}
!isEmpty(target.path): INSTALLS += target

# BIN     = $(DESTDIR)/usr/bin
# ICONS   = $(DESTDIR)/usr/share/icons/hicolor/64x64/apps
