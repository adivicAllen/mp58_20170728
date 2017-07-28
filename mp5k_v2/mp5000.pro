#-------------------------------------------------
#
# Project created by QtCreator 2017-07-11T14:41:49
#
#-------------------------------------------------

QT       -= gui

TARGET = mp5000
TEMPLATE = lib

DEFINES += MP5000_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        mp5000.cpp \
    ADIVIC_Logger.cpp \
    ADIVIC_Sync_TelnetClient.cpp \
    AdivicTClient.cpp \
    BoostClient.cpp \
    dllmain.cpp \
    mp5k_v2.cpp \
    mp5000.cpp \
    stdafx.cpp \
    Sync_TelnetClient.cpp

HEADERS += \
        mp5000.h \
        mp5000_global.h \ 
    ADIVIC_Logger.h \
    ADIVIC_Sync_TelnetClient.h \
    adivic_thread.hpp \
    AdivicGlobal.hpp \
    AdivicTClient.h \
    BoostClient.h \
    define.h \
    Dut_define.h \
    MFG_INCLUDE.h \
    MFG_TOOL_Define.h \
    mp5k_define.h \
    mp5k_error.h \
    mp5k_v2.h \
    mp5000.h \
    mp5000_global.h \
    MP5000_SCPI.h \
    resource.h \
    stdafx.h \
    Sync_TelnetClient.h \
    targetver.h \
    TOP_LEVEL.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
