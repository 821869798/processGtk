QT += core
QT -= gui

CONFIG += c++11

TARGET = gtkTest
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    main.c

CONFIG +=  link_pkgconfig
PKGCONFIG += gtk+-2.0

HEADERS += \
    process.h
