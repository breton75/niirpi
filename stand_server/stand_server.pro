QT -= gui
QT += serialport sql

CONFIG += c++14 console
CONFIG -= app_bundle

TARGET = stand_server

DEFINES += APP_DEBUG=1

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    ../../svlib/sv_sqlite.cpp \
    ../oht/sv_oht.cpp \
    ../../svlib/sv_clog.cpp \
    sv_dbgate.cpp \
    ../global/sv_signal.cpp \
    ../../svlib/sv_pgdb.cpp

HEADERS += \
    ../../svlib/sv_exception.h \
    ../../svlib/sv_sqlite.h \
    ../global/sv_idevice.h \
    ../oht/sv_oht.h \
    ../../svlib/sv_clog.h \
    ../global/sql_defs.h \
    sv_dbgate.h \
    ../global/sv_signal.h \
    ../../svlib/sv_pgdb.h
