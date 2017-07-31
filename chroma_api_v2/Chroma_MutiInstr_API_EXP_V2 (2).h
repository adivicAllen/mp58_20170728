#pragma once

#ifdef _CHROMA_API_EXPORTS_LIB
#define CHROMA_API_DLLEXT  extern "C" __declspec(dllexport)
#else
#define CHROMA_API_DLLEXT  extern "C" __declspec(dllimport)
#endif

typedef enum{
	Port_1=0,
	Port_2,
	Port_3,
	Port_4,
	Port_5,
	Port_6,
	Port_7,
	Port_8,
}PORT_Select ;

typedef enum{
	RBW_100_Hz=0,
	RBW_500_Hz,
	RBW_1K_Hz,
	RBW_2K_Hz,
	RBW_5K_Hz,
	RBW_10K_Hz,
	RBW_20K_Hz,
	RBW_50K_Hz,
	RBW_100K_Hz,
	RBW_200K_Hz,
	RBW_500K_Hz,
	RBW_1M_Hz,
	RBW_2M_Hz,
	RBW_5M_Hz,
}RBW_Set;


typedef enum{
	Switch_OFF,
	Switch_RX,
	Switch_TX
}PORT_Mode;

typedef enum{
	FSK_2,
	FSK_4,
	FSK_8
}FSK_TYPE;


CHROMA_API_DLLEXT const char* __stdcall expMV2_GetLastErrorMessage();

CHROMA_API_DLLEXT USHORT __stdcall expMV2_InitFunction(const char *IniFilePath);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_ConnectSever(PORT_Select PortIndex,const char * HostAddress, const char * Port,unsigned int uiTimeout=10);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetSeverIsConnect(PORT_Select PortIndex,bool &bIsConnect);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_UploadWaveform(const char *WaveformFilePath);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_PresetServer(PORT_Select PortIndex);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_OpenServer();
CHROMA_API_DLLEXT USHORT __stdcall expMV2_CloseServer(PORT_Select PortIndex);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_CloseAllServer();
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SaveVsaData(PORT_Select PortIndex,const char *SaveName);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_InputCableLoss(PORT_Select PortIndex,const char *InstrCableLossFilePath);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetCableLossEnable(PORT_Select PortIndex,const bool bEnable);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetDistributionModeOnOff(PORT_Select PortIndex,const bool bOnOff);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetCenterFrequency(PORT_Select PortIndex,const float fFreqMHz,const float fFreqOffsetMHz=0); //50~6000 MHz
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetCenterFrequency(PORT_Select PortIndex,float &fFreqMHz);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetFreqSpan(PORT_Select PortIndex,const int iFreqSpanMHz);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetFreqSpan(PORT_Select PortIndex,int &iFreqSpanMHz);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetRxRBW(PORT_Select PortIndex,const RBW_Set uiEnum);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetRxRBW_Int(PORT_Select PortIndex,const int iIndex);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetRxRBW(PORT_Select PortIndex,UINT64 &uiRBW_Hz);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_PowerReferenceAuto(PORT_Select PortIndex);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetRxReferenceLevel(PORT_Select PortIndex,const float fRefLevel);//10~-80 dBm
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetRxReferenceLevel(PORT_Select PortIndex,float &fRefLevel);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetPeakData(PORT_Select PortIndex,float &fPeakFreqMHz,float &fPeakPower);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetGFSK_FreqDeviation(PORT_Select PortIndex,float &fFreqDevMHz);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetVsaRx_Port(const PORT_Select PortIndex);
//Switch_OFF,Switch_RX,Switch_TX
CHROMA_API_DLLEXT USHORT __stdcall expMV2_Set_4Port_Switch(const PORT_Select PortFirstIndex,const PORT_Mode pm_Port_1,
																							const PORT_Mode pm_Port_2, 
																							const PORT_Mode pm_Port_3,
																							const PORT_Mode pm_Port_4);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetTxOutputLevel(PORT_Select PortIndex,const float fOutputLevel);//10~-90 dBm
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetTxOutputLevel(PORT_Select PortIndex,float &fOutputLevel);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetCWFrequencyOffset(PORT_Select PortIndex,const float fFreqOffsetMHz); //-50~50MHz


CHROMA_API_DLLEXT USHORT __stdcall expMV2_LoadMemory(PORT_Select PortIndex,const char * strFileName);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_TxPlayRepeatOnOff(PORT_Select PortIndex,const bool bOnOff);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetCurrentFreqLoss(const float fFreq,const PORT_Select PortIndex,float& fTxLoss,float& fRxLoss);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetChannelPower(PORT_Select PortIndex,float &fChannelPower);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_ModulationOnOff(PORT_Select PortIndex,const bool bModulationOnOff);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_Rf_OnOff(PORT_Select PortIndex,const bool bRfOnOff);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_Get_Rf_OnOff(PORT_Select PortIndex,bool& bRfOnOff);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetVsaOnOff(PORT_Select PortIndex,const bool bOnOff);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetMark(PORT_Select PortIndex,const float fMarkFreqMHz);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetMarkValue(PORT_Select PortIndex,float &fMarkValue);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_ClearMark(PORT_Select PortIndex);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_DemodFskOpen(PORT_Select PortIndex,bool bIsOpen);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_DemodFskSetting(PORT_Select PortIndex,bool bIsDemod);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetSpectrumOnOff(PORT_Select PortIndex,const bool bOnOff);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetCableLossTableCoupleMode(PORT_Select PortIndex,const bool bOnOff);
