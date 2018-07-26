#-------------------------------------------------
#
# Project created by QtCreator 2018-07-17T11:15:35
#
#-------------------------------------------------

QT       += core gui xml sql serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = configurator
TEMPLATE = app

VERSION = 0.1.0    # major.minor.patch
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

DEFINES += APP_DEBUG=1

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    ../../svlib/sv_settings.cpp \
    ../../svlib/sv_sqlite.cpp \
    treeitem.cpp \
    treemodel.cpp \
    sv_device_type_editor.cpp \
    sv_device_editor.cpp \
    ../../svlib/sv_log.cpp

HEADERS += \
        mainwindow.h \
    ../../svlib/sv_exception.h \
    xbelreader.h \
    xbelwriter.h \
    ../../svlib/sv_settings.h \
    ../../svlib/sv_sqlite.h \
    ../global/sql_defs.h \
    treeitem.h \
    treemodel.h \
    sv_oht.h \
    sv_idevice.h \
    sv_device_type_editor.h \
    sv_device_editor.h \
    ../../svlib/sv_log.h

FORMS += \
        mainwindow.ui \
    sv_device_type_editor.ui \
    sv_device_editor.ui

RESOURCES += \
    res.qrc
