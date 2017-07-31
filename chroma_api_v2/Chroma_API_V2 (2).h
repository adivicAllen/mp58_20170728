#ifndef CHROMA_API_V2_H
#define CHROMA_API_V2_H

#include "AdivicTClient.h"
#include <string>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <atlstr.h>
#include <WinBase.h>
#include <math.h>
#include "Global_ErrorCode_Def.h"
#include "Chroma_MutiInstr_API_EXP_V2.h"
#include "MP5K_SCPI.h"
#include <vector>
#include "Dll_Common.h"
#include "IniSetting.h"





// This class is exported from the Chroma_API.dll
class CChroma_API_V2
{
public:
	CChroma_API_V2(void);
	~CChroma_API_V2();



	// TODO: add your methods here.
	static bool s_bIsOpenProfile;
	static CString s_szConfigFilePath;
	static bool s_bIsServer_Connected[MaxInstr];
//-----------------------------------------------------
	static bool s_bIsServer_OnOff[MaxInstr];
	static CString s_szServer_IP[MaxInstr];
	static CString s_szServer_Port[MaxInstr];//Port Number
	//static bool s_bEnableLossFileNotExist;
	static CString s_szServer_PortLoss[MaxInstr];//PortLoss File Name
	static vector<CString> s_vec_WaveformPath;
//------------------------------------------------------------

	static char s_ErrMessage[1024];

	static IniSetting inisetting;

	USHORT usInstrIndex;

	MP5K_SCPI Adivic_MP5K;
	string stdStr_SendCommand;
	string stdStr_ReadRespond;
	string stdStr_LoadFileName;
	string stdStr_IP_Address;
	bool bIsServerConnectFlag;//Single One server
	char readBuf[1024];
	//Cable Loss
	vector<LossPoint> vec_DutCableLossList;
	bool m_bDutCableLossEnable;//Load Cable loss file 

	float m_f_Center_CurrentFreq;
	bool m_PortLossEnable[Port_Nums];
	USHORT m_CurrentVsaPort; //Use to calculate cable Loss:0~3 

	//Current Setting
	bool m_bModulationOnOff;
	bool m_bRF_OnOff; 
	float m_CenterFrequency;
	float m_fMarkOffset;
	int m_iFreqSpanMHz;
	unsigned int m_uiRBW_Enum;
	float m_fRefLevel;
	unsigned int m_uiVsa_Port;
	//Demodulation setting
	USHORT m_usDemodTool_Index;
	USHORT m_usFSK_SetType,m_usFSK_SampleRateMHz;
	USHORT m_usFSK_FilterType,m_usFSK_FilterLength;
	float m_fFskcoefficient;
	USHORT m_usFskSymbolPhaseContinuity;

	// -----------
	byte byteRunStatus;
	byte byteRfOnStatus;
	byte byteModulationStatus;

	static const int _ON  = 0;
	static const int _OFF  = 1;
	static const int _ERROR = 2;



	//Information
	float m_Current_Loss[Port_Nums];

	bool TestFunction(); //-----Test
	USHORT InitFunction(const char *IniFilePath);//////////////////////////////////////////////////////////////////////////
	USHORT ConfingFileParser();
	USHORT LoadPortDifference(CString szFilePath);
	USHORT LoadCableLoss(CString szFilePath);
	USHORT NetConnect(bool bIsConnect);
	USHORT UploadWaveform(const char *WaveformPath,bool bIsOverLoad=false);
	const char* GetLastErrorMessage();
	void SetCurrentErrorMessage(USHORT usErrorCode);
	USHORT PresetFunc();
	bool SaveVsaData(const string *strTableName);
	//////////////////////////////////////////////////////////////////////////
	
	bool SortLossPointTable(vector<LossPoint>& LossPointTable);

	bool FindFreqLoss(float fFreq,const vector<LossPoint>& LossPointTable,LossPoint& loss_point);
	bool SetFreqPointTxOutputLevelAndPortAtt(const float fOutputLevel);
	bool GetCurrentFreqLoss(const float fFreq,const unsigned int uiPortIndex,float& fTxLoss,float& fRxLoss);
	void GetDutCurrentTxLoss(float fFreq,float& fTxLoss);
	
	void CurrentFilePath(char * chFileDir);

	bool ConnectSever(const char * host, const char * port = "23",unsigned int uiTimeout=10);
	void CloseServer();
	//CableLoss
	bool SetDistributionModeOnOff(bool bOnOff);
	void GetDistributionModeOnOff(bool &bOnOff);

	bool SetCurrentTableName(unsigned int uiPortIndex,bool bIsTx);
 	void GetCurrentSetTableName(string &strTableName);
 
 	bool SetCurrentLossPoint(const float fFreqMHz,const float fLoss);
 	bool GetCurrentFreqLoss(const float fFreqMHz,float &fLoss);
 
 	bool ClearCurrentAllLossData();//Clear Memory Loss Data 
 	USHORT SetAllPortLossFile(bool bIsClear);
	

	bool SaveCurrentLossFileToInstr();
	bool DeleteLossFileInInstr(unsigned int uiPortIndex,bool bIsTx);
	USHORT ResetInstrCableLossSetting();
	//-------------------------------------------------------------------------------	
	
	bool SetCenterFrequency(const float fFreqMHz);
	void GetCenterFrequency(float &fFreqMHz); 

	bool SetFreqSpan(const int iFreqSpanMHz);
	void GetFreqSpan(int &iFreqSpanMHz);

	bool SetRxRBW(const RBW_Set uiEnum);
	void GetRxRBW(UINT64 &uiRBW_Hz);

	bool PowerReferenceAuto();

	bool SetRxReferenceLevel(const float fRefLevel);
	void GetRxReferenceLevel(float &fRefLevel);

	void GetPeakData(float &fPeakFreq,float &fPeakPower);

	void GetGFSK_FreqDeviation(float &fFreqDevMHz);
	
	bool SetVsaRx_Port(const unsigned int uiPortIndex); // Vaule:1~4
	bool Set_4Port_Switch(const PORT_Mode uiPort_1,const PORT_Mode uiPort_2,const PORT_Mode uiPort_3,const PORT_Mode uiPort_4);
	
	//---------------------------------------------------------------
	bool SetTxOutputLevel(const float fOutputLevel);
	void GetTxOutputLevel(float &fOutputLevel);

	bool SetCWFrequencyOffset(const float fOffsetMHz);
	void GetCWFrequencyOffset(float &fOffsetMHz);

	bool LoadPlayFile(const char* strFileName);
	
	bool SetVsgRfOnOff(const bool bRfOn);
	void GetVsgRfOnOff(bool &bRfOn);

	bool SetVsgModulationOnOff(const bool bModOn);
	void GetVsgModulationOnOff(bool &bModOn);

	bool TxPlayRepeatOnOff(const bool bOnOff);
	void GetChannelPower(float &fChannelPower);
	
	bool SetVsgOnOff(const bool bOnOff);
//	void GetVsgOnOff(bool &bOnOff);

	bool SetVsaOnOff(const bool bOnOff);

	bool SetMark(const float fMarkFreqMHz);
	void GetMarkValue(float &fMarkValue);
	bool ClearMark(void);

	//FSK
	bool SetDemodulationtype(USHORT usType=1);
	bool FskSetType(USHORT usType=2);
	bool FskSetSymbolPhaseContinuity(bool bIsContinue);
	bool FskSetSymbolRate(USHORT usMHz=1);
	void FSK_GetFreqDeviation(float &fFreqDevMHz);
	bool FskSetFilterType(USHORT usType);
	bool FskSetFilterLength(USHORT usLength);
	bool FskSetcoefficient(float fCofficient);
	bool SetDemodulationTool(USHORT usIndex);

	// set, get spectrum mode

	bool SetSpectrumOnOff(const bool bModOn);
	void GetSpectrumOnOff(bool &bModOn);

	// set, get cable loss table mode
	bool SetCableLossTableCoupleMode(const bool bModOn);
	void GetCableLossTableCoupleMode(bool &bModOn);

};


#endif