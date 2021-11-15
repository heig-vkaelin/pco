QT += core
QT -= gui

CONFIG += c++17

TARGET = test
CONFIG -= app_bundle

TEMPLATE = app

LIBS += -lgtest -lpcosynchro

SOURCES += main.cpp

HEADERS += \
    pcobarrier.h
