#ifndef MP5K_SCPI_H
#define MP5K_SCPI_H

#include "ADIVIC_Sync_TelnetClient.h"
#include "D:\\DSPXXX_API\\Chroma_API_V2\\MP5800_API_define.h"
//#include "AdivicTClient.h"
//#include "AdivicSocket.h"
#include <string>
using namespace std;
#include <string>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <atlstr.h>
#include <WinBase.h>
#include <math.h>
#include <vector>

//------SCPI Read Setting
#define WaitSegTime 50
// #define WaitTimesMAX 30 // time out of read , modify from 200 -> 500 -> 1500 
#define WaitTimesMAX 1  // allen test
#define MP5K_Port_Nums	4

#define FSK_SymboRate_Min 20000
#define FSK_SymboRate_Max 30000000

typedef enum{
	_MOD_GPRF,
	_MOD_OFDM,
	_MOD_DSSS,
	_MOD_BLUETOOTH,
	_MOD_Error
}Mea_Mode;

typedef enum{
	_FMOD_None,
	_FMOD_Raise_cosine,
	_FMOD_Root_raise_cosine,
	_FMOD_Gaussian,
}Filter_Mode;



class MP5K_SCPI
{
public:
	MP5K_SCPI(void);
	~MP5K_SCPI();

	USHORT usInstrIndex;
	ADIVIC_Sync_TelnetClient  advClient;  // from eric
//	void setMachineID(int ID);
//	AdivicTClient advClient;
	//AdivicSocket  advClient;
	string stdStr_SendCommand;
	string stdStr_ReadRespond;
	string stdStr_LoadFileName;
	static bool bIsConnectFlag;	//Single One server
	char readBuf[1024];
	char m_SendBuf[1024];
 	double m_f_Center_CurrentFreq;
	bool ConnectSever(const char * host, const char * port = "23",unsigned int iTimeout=5);
	void CloseServer();
	bool ReadRespond(unsigned int &uiRead);
	bool WaitUntilReapond();

	bool GetTestRawData();
	bool SaveVsaData(const string *strTableName);

	//Common
	bool SetCenterFrequency(const double fFreqMHz);
	void GetCenterFrequency(double &fFreqMHz); 

	bool SetMeasureMode(const Mea_Mode CurrentMode);
	void GetMeasureMode(Mea_Mode &CurrentMode);

	bool SetPortMode(unsigned int uiPort,bool bIsVsg);//Port 1~4
	bool Set_4_PortModeIsVsg(bool bPORT_1,bool bPort_2,bool bPort_3,bool bPort_4);
// 	bool Set_4_PortMode_at_1_Time(const UINT uiPORT_1,const UINT uiPort_2,const UINT uiPort_3,const UINT uiPort_4);
// 	bool Set_4_PortMode_at_4_Time(const UINT uiPORT_1,const UINT uiPort_2,const UINT uiPort_3,const UINT uiPort_4);
	bool Set_4_PortMode(const UINT uiPORT_1,const UINT uiPort_2,const UINT uiPort_3,const UINT uiPort_4);
	//Cable Loss
	bool SetDistributionModeOnOff(bool bOnOff);
	void GetDistributionModeOnOff(bool &bOnOff);

	bool SetCurrentTableName(const string *strTableName);
	void GetCurrentSetTableName(string &strTableName);
	bool SetVsaTriggerType(const MP5800::MP5800_VSA_Trigger_Type triggerType);
	bool SetCurrentLossPoint(const double fFreqMHz,const double fLoss);
	bool GetCurrentFreqLoss(const double fFreqMHz,double &fLoss);

	bool ClearCurrentAllLossData();//Clear Memory Loss Data 
	bool SetPortModeLossFile(unsigned int uiPort,bool bIsTx,string strTableName);

	bool SaveLossFileToInstr(string strTableName);
	bool DeleteLossFileInInstr(const string& strTableName);
	//Measure-----------------------------------------------------------------------
	bool GetMeasureRawData(); //Execute before every Measure time
	void GetPeakPowerData(double &fFreqMHz,double &fPower,int iWindowIndex=4); // allen 0728
	void GetGFSK_FreqDeviation(double &fFreqDevMHz);
	void MeasureGFSK(double &dFreqDevMHz,double &dFreqOffset,double &dFreqDrift,double &dDevError,double &dFskError);
	//=========================================================================
	void GetChannelPower(double &fChPower);
	void GetChannelPowerAverage(double &fChPowerAvge);
	void GetChannelPowerMax(double &fChPowerMax);
	void GetChannelPowerMin(double &fChPowerMin);

	void GetChannelPowerPeak(double &fChPowerPeak);
	void GetChannelPowerPeakAverage(double &fChPowerPeakAvge);
	void GetChannelPowerPeakMax(double &fChPowerPeakMax);
	void GetChannelPowerPeakMin(double &fChPowerPeakMin);
	//VSA---------------------------------------------------------------------------
	bool SetVsaStartOnOff(const bool bIsStart);
	//Span
	bool SetVsaSampleRate(const int fSampleRateMHz); 
	void GetVsaSampleRate(int &iSampleRateMHz);
	
	bool SetVsaRBW(const UINT64 uiRBW_Hz);
	void GetVsaRBW_Hz(UINT64 &uiRBW_Hz);
	bool SetVsaGapTime(const double dCapTime_us);
	bool SetVsaReferenceLevel(const double fRefLevel);
	void GetVsaReferenceLevel(double &fRefLevel);
	bool VsaReferenceLevelAuto();
	bool SetVsaCaptureLength(const double dCaptureLength_ms);
 	bool SetVsaTriggerTimeOut(const double dTriggerTimeOut_ms);
	bool SetVsaCaptureTime(const double dRate_S);//Max 10s,Min 0.000001
	void GetVsaCaptureTime(double &fRate_S);

	bool SetVsaCaptureCount(const UINT uiCountNum);
	void GetVsaCaptureCount(UINT &uiCountNum);

	bool SetVsaSkipCaptureCount(const UINT uiSkipNum);
	void GetVsaSkipCaptureCount(UINT &uiSkipNum);

	bool SaveMeasureWaveForm(const char * WaveFormName);
	bool SetVsaTriggerOffsetUs(const int OffsetUs);
	bool AddMarkerFreqOffset(const double fFreqOffsetMHz);
	void GetCWChannelPower(const double fMarkFreqMHz, const double leftSpan_Hz, const double rightSpan_Hz, 
	                 double &PowerMeasured, double &PowerMeasured_Left, double &PowerMeasured_Right, double &Power_Comp, double &Frequency_Offset );


	void GetMarkerPower(double &fPower);
	void GetPowerByTime(double &dPowerByTime);
	void GetPeakPowerFrequencyHzByTime(double &dPeakPowerFrequencyHzByTime);
	bool ClearMarker();
	// ---
	bool SetDigitalDownConvertor(const bool bOnOff,  const double offSet_Hz );
	void GetChannelPower2Span(const double fMarkFreqMHz, const double leftSpan_Hz, const double rightSpan_Hz,double &PowerMeasured, double &PowerMeasured_Left, double &PowerMeasured_Right );

	//---GFSK
//	bool SetDemodulationtype(bool bIsDeMode);
	
    bool SetDemodulationtype(USHORT usType);
	bool SetDemodulationTool(USHORT usSloution);
	//bool FskSetType(USHORT usType=2);
	bool FskSetType(USHORT usFskType);
	void FskGetType(USHORT &usType);
	//bool FskSetSymbolPhaseContinuity(bool bIsContinue);
	bool FskSetSymbolPhaseContinuity(USHORT usCountinuity);
	bool FskSetSymbolRate(unsigned int uiRateHz);
	bool FSK_SetFreqDeviation(unsigned int uiFreqDev_Hz);
	void FSK_GetFreqDeviation(double &fFreqDevMHz);
	bool FskSetFilterType(Filter_Mode _filterMode);
	bool FskSetFilterLength(USHORT usLength);
	bool FskSetcoefficient(double fCofficient);

	
	

	//VSG---------------------------------------------------------------------------
	bool SetVsgRfStartStop(const bool bVsgStart);

	bool SetVsgOutputLevel(const double fOutputLevel);
	void GetVsgOutputLevel(double &fOutputLevel);

	bool SetVsgPlayFileName(const char * PlayFileName);
	bool SetVsgPlayFileNameWithFilePath(const char * PlayFileName, const char *filePathName);
	
	bool SetVsgPlayTimes(const UINT uiTimes);
	void GetVsgPlayTimes(UINT &uiTimes);
	bool SetVsgPlayGapUs(const UINT GapUs);

	

	bool SetVsgModulationOnOff(const bool bModOn);
	void GetVsgModulationOnOff(bool &bModOn);

	bool SetVsgRfOnOff(const bool bRfOn);
	void GetVsgRfOnOff(bool &bRfOn);

	bool Set_CW_Offset(const double fOffsetMHz);
	void Get_CW_Offset(double &fOffsetMHz);

	 
	bool SetSpectrumOnOff(const bool bModOn);
	void GetSpectrumOnOff(bool &bModOn);

	bool SetCableLossTableCoupleMode(const bool bModOn);
	void GetCableLossTableCoupleMode(bool &bModOn);

	bool SetFilterWindow(const UINT filterType);
 
};


#endif