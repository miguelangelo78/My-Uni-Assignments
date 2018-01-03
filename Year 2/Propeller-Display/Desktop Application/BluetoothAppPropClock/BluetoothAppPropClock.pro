#-------------------------------------------------
#
# Project created by QtCreator 2016-02-22T05:31:23
#
#-------------------------------------------------

QT       += core gui bluetooth serialport
CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BluetoothAppPropClock
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    serialhandler.cpp

HEADERS  += mainwindow.h \
    serialhandler.h \
    serialio.h

LIBS += -lshell32

FORMS    += mainwindow.ui
