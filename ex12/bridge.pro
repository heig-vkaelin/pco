#-------------------------------------------------
#
# Project created by QtCreator 2015-04-15T17:25:03
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++17

TEMPLATE = app

LIBS += -lgtest -lpcosynchro



SOURCES += main.cpp

INCLUDEPATH += ../
HEADERS += \
    bridgemanager.h
