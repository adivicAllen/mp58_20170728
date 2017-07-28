QT += core
QT -= gui

TARGET = qtLibTest
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += /home/allen/libtest
LIBS +=  /home/allen/build-libtest-Desktop_Qt_5_5_1_GCC_32bit-Debug/liblibtest.so
#         /home/allen/build-libtest-Desktop_Qt_5_5_1_GCC_32bit-Debug/liblibtest.so.1
SOURCES += main.cpp

QMAKE_LFLAGS += -Wl,-rpath,"'$$ORIGIN'"
#QMAKE_LFLAGS += -Wl,-rpath,"'\$$ORIGIN'"
