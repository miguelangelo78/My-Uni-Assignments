QT = core
QT += serialport

CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TARGET = bluetooth_driver

TEMPLATE = app

SOURCES += main.cpp \
    serialhandler.cpp

HEADERS += \
    serialhandler.h
