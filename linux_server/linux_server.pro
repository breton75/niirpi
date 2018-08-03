#-------------------------------------------------
#
# Project created by QtCreator 2014-11-14T17:46:03
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = linux_server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    MyServer.cpp \
    ../Common/tthread.cpp \
    console_socket.cpp \
    ../Common/console_log.cpp \
    iothread.cpp

HEADERS += \
    MyServer.h \
    ../Common/tthread.h \
    console_socket.h \
    ../Common/console_log.h \
    consoleapplication.h \
    iothread.h
