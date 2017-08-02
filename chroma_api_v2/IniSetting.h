#pragma once
#include <string>
#include <fstream>
#include <sstream>
// #include <Windows.h>
// #include <atlstr.h>
// #include <WinBase.h>
#include <math.h>
#include <vector>
#include "Dll_Common.h"
#include "Global_ErrorCode_Def.h"
#include <QString>

using namespace std;

class IniSetting
{
public:
	IniSetting(void);
	~IniSetting(void);

    QString s_szConfigFilePath;

	
    QString s_szServer_IP[MaxInstr];
    QString s_szServer_Port[MaxInstr]; //Port Number
	bool s_bIsServer_OnOff[MaxInstr];
    QString s_szServer_PortLoss[MaxInstr]; //PortLoss File Name
    vector<QString> s_vec_WaveformPath;

    USHORT SetConfigFilePath(QString csFilePath);
	USHORT ParserConfigFile();
	USHORT WriteConfigFile();


};

