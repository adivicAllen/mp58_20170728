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
    Chroma_API_V2.cpp \
    Chroma_MutiInstr_API_EXP_V2.cpp \
    IniSetting.cpp \
    mp5k.cpp \
    MP5K_SCPI.cpp

HEADERS += chroma_api_v2.h\
        chroma_api_v2_global.h \
    AdivicSocket.h \
    Chroma_API_V2.h \
    Chroma_MutiInstr_API_EXP_V2.h \
    define.h \
    Dll_Common.h \
    Global_Constant_Def.h \
    Global_ErrorCode_Def.h \
    IniSetting.h \
    mp5k.h \
    mp5k_define.h \
    MP5K_SCPI.h \
    MP5800_API_const.h \
    MP5800_API_define.h

DEFINES += _DEBUG    \
           _USRDLL   \
           _CHROMA_API_EXPORTS_LIB \
           _WINDLL    \
           _UNICODE   \
            UNICODE   \

DESTDIR = $$_PRO_FILE_PWD_/../lib
message($$_PRO_FILE_)
message($$_PRO_FILE_PWD_)


unix {
    target.path = /usr/lib
    INSTALLS += target
}

