#-------------------------------------------------
#
# Project created by QtCreator 2017-07-31T11:48:11
#
#-------------------------------------------------

QT       -= gui

TARGET = chroma_API_v2
TEMPLATE = lib

DEFINES += CHROMA_API_V2_LIBRARY
INCLUDEPATH +=/home/allen/mp58_20170728/MP5800_API \
              /home/allen/mp58_20170728/mp5k_v2    \
              /home/allen/boost_1_61_0

SOURCES += chroma_api_v2.cpp \
    ../mp58_20170728/chroma_api_v2/Chroma_API_V2.cpp \
#    ../mp58_20170728/chroma_api_v2/Chroma_API_V2 (2).cpp \
    ../mp58_20170728/chroma_api_v2/Chroma_MutiInstr_API_EXP_V2.cpp \
#    ../mp58_20170728/chroma_api_v2/Chroma_MutiInstr_API_EXP_V2 (2).cpp \
#    ../mp58_20170728/chroma_api_v2/dllmain.cpp \
#    ../mp58_20170728/chroma_api_v2/dllmain (2).cpp \
    ../mp58_20170728/chroma_api_v2/IniSetting.cpp \
#    ../mp58_20170728/chroma_api_v2/IniSetting (2).cpp \
    ../mp58_20170728/chroma_api_v2/mp5k.cpp \
    ../mp58_20170728/chroma_api_v2/MP5K_SCPI.cpp
#    ../mp58_20170728/chroma_api_v2/stdafx.cpp \
#    ../mp58_20170728/chroma_api_v2/stdafx (2).cpp

HEADERS += chroma_api_v2.h\
        chroma_api_v2_global.h \
    ../mp58_20170728/chroma_api_v2/AdivicSocket.h \
#    ../mp58_20170728/chroma_api_v2/AdivicSocket (2).h \
    ../mp58_20170728/chroma_api_v2/Chroma_API_V2.h \
#    ../mp58_20170728/chroma_api_v2/Chroma_API_V2 (2).h \
    ../mp58_20170728/chroma_api_v2/Chroma_MutiInstr_API_EXP_V2.h \
#    ../mp58_20170728/chroma_api_v2/Chroma_MutiInstr_API_EXP_V2 (2).h \
    ../mp58_20170728/chroma_api_v2/define.h \
    ../mp58_20170728/chroma_api_v2/Dll_Common.h \
#    ../mp58_20170728/chroma_api_v2/Dll_Common (2).h \
    ../mp58_20170728/chroma_api_v2/Global_Constant_Def.h \
    ../mp58_20170728/chroma_api_v2/Global_ErrorCode_Def.h \
    ../mp58_20170728/chroma_api_v2/IniSetting.h \
 #   ../mp58_20170728/chroma_api_v2/IniSetting (2).h \
    ../mp58_20170728/chroma_api_v2/mp5k.h \
    ../mp58_20170728/chroma_api_v2/mp5k_define.h \
    ../mp58_20170728/chroma_api_v2/MP5K_SCPI.h \
    ../mp58_20170728/chroma_api_v2/MP5800_API_const.h \
    ../mp58_20170728/chroma_api_v2/MP5800_API_define.h \
    ../mp58_20170728/chroma_api_v2/resource.h
#    ../mp58_20170728/chroma_api_v2/resource (2).h \
#    ../mp58_20170728/chroma_api_v2/stdafx.h \
#    ../mp58_20170728/chroma_api_v2/stdafx (2).h \
#    ../mp58_20170728/chroma_api_v2/targetver.h \
#    ../mp58_20170728/chroma_api_v2/targetver (2).h

DEFINES += _DEBUG    \
           _USRDLL   \
           _CHROMA_API_EXPORTS_LIB \
           _WINDLL    \
           _UNICODE   \
            UNICODE   \

unix {
    target.path = /usr/lib
    INSTALLS += target
}
