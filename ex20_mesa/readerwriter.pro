
TEMPLATE = app
TARGET = test
INCLUDEPATH += .
CONFIG += debug
CONFIG += c++17

LIBS += -lgtest -lpcosynchro

# Input
SOURCES += readerwriter.cpp

HEADERS += \
    abstractreaderwriter.h \
    readerwriterpriowritergeneral.h
