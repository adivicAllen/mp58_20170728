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

using namespace std;

class IniSetting
{
public:
	IniSetting(void);
	~IniSetting(void);

	CString s_szConfigFilePath;

	
	CString s_szServer_IP[MaxInstr];
	CString s_szServer_Port[MaxInstr]; //Port Number
	bool s_bIsServer_OnOff[MaxInstr];
	CString s_szServer_PortLoss[MaxInstr]; //PortLoss File Name
	vector<CString> s_vec_WaveformPath;

	USHORT SetConfigFilePath(CString csFilePath);
	USHORT ParserConfigFile();
	USHORT WriteConfigFile();


};

