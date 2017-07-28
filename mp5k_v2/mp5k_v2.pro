QT       += core
QT       -= gui

TEMPLATE = lib
CONFIG += c++11
INCLUDEPATH += /home/allen/boost_1_61_0

SUBDIRS += \
    ../mp5000.pro

DISTFILES += \
    ../mp5000.pro.user \
    ../config \
    ../description \
    ../HEAD \
    ../README.md

HEADERS += \
    ../AdivicGlobal.hpp \
    ../ADIVIC_Logger.h \
    ../ADIVIC_Sync_TelnetClient.h \
    ../adivic_thread.hpp \
    ../BoostClient.h \
    ../define.h \
    ../Dut_define.h \
    ../MFG_INCLUDE.h \
    ../MFG_TOOL_Define.h \
    ../mp5k_define.h \
    ../mp5k_error.h \
    ../mp5k_v2.h \
    ../mp5000.h \
    ../mp5000_global.h \
    ../MP5000_SCPI.h \
    ../resource.h \
    ../Sync_TelnetClient.h \
    ../targetver.h \
    ../TOP_LEVEL.h \
    ../types.h

SOURCES += \
    ../ADIVIC_Logger.cpp \
    ../ADIVIC_Sync_TelnetClient.cpp \
    ../BoostClient.cpp \
    ../mp5k_v2.cpp \
    ../Sync_TelnetClient.cpp


LIBS += -L"/home/allen/boost_1_61_0/stage/lib/"
DEFINES += _WINDLL \
           MFG_LOG \
#            WIN32   \
           _DEBUG  \
#          _WINDOWS  \
          _USRDLL  \
          MP5K_V2_EXPORTS  \
         _UNICODE     \
         UNICODE
