QT += core
QT -= gui

CONFIG += c++17

TARGET = test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

LIBS += -lgtest -lpcosynchro

SOURCES += main.cpp

HEADERS += \
    mutexfromsem.h
