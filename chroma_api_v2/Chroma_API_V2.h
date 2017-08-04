#ifndef CHROMA_API_V2_H
#define CHROMA_API_V2_H

// #include "AdivicTClient.h"
#include <string>
#include <fstream>
#include <sstream>
//#include <Windows.h>
//#include <atlstr.h>
//#include <WinBase.h>
#include <math.h>
#include "Global_ErrorCode_Def.h"
#include "Chroma_MutiInstr_API_EXP_V2.h"
#include "MP5K_SCPI.h"
#include <vector>
#include "Dll_Common.h"
#include "IniSetting.h"
#include "mp5k.h"
#include <QString>
#define sprintf sprintf_s
 




// This class is exported from the Chroma_API.dll
class CChroma_API_V2
{
public:
	CChroma_API_V2(void);
	~CChroma_API_V2();

	static std::string IP;
	static unsigned short port;


	// TODO: add your methods here.
	static bool s_bIsOpenProfile;
    static QString s_szConfigFilePath;
	static bool s_bIsServer_Connected[MaxInstr];
//-----------------------------------------------------
	static bool s_bIsServer_OnOff[MaxInstr];
    static QString s_szServer_IP[MaxInstr];
    static QString s_szServer_Port[MaxInstr];//Port Number
	//static bool s_bEnableLossFileNotExist;
    static QString s_szServer_PortLoss[MaxInstr];//PortLoss File Name
    static vector<QString> s_vec_WaveformPath;
//------------------------------------------------------------

	static char s_ErrMessage[1024];

	static IniSetting inisetting;

	USHORT usInstrIndex;
	MP5800_STANDARD measureMode;
	MP5K_SCPI Adivic_MP5K;
	mp5k      Adivic_MP5KWifiBT;  // Allen
	UINT handle; // MP5K Tester Handle
    UINT ErrorCode;
	string stdStr_SendCommand;
	string stdStr_ReadRespond;
	string stdStr_LoadFileName;
	string stdStr_IP_Address;
	bool bIsServerConnectFlag;//Single One server
	char readBuf[1024];
	//Cable Loss
	vector<LossPoint> vec_DutCableLossList;
	bool m_bDutCableLossEnable;//Load Cable loss file 

	double m_f_Center_CurrentFreq;
	bool m_PortLossEnable[Port_Nums];
	USHORT m_CurrentVsaPort; //Use to calculate cable Loss:0~3 

	//Current Setting
	bool m_bModulationOnOff;
	bool m_bRF_OnOff; 
	double m_CenterFrequency;
	double m_fMarkOffset;
	int m_iFreqSpanMHz;
	unsigned int m_uiRBW_Enum;
	unsigned int m_uiTriggerType_Enum;
	double m_fRefLevel;
	double m_dCaptureLength_ms;
	double m_dTriggerTimeOut_ms;
	double m_dGapTime_us;
	unsigned int m_uiVsa_Port;
	//Demodulation setting
	USHORT m_usDemodTool_Index;
	USHORT m_usFSK_SetType,m_usFSK_SampleRateMHz;
	USHORT m_usFSK_FilterType,m_usFSK_FilterLength;
	double m_fFskcoefficient;
	USHORT m_usFskSymbolPhaseContinuity;

	// -----------
	byte byteRunStatus;
	byte byteRfOnStatus;
	byte byteModulationStatus;

	static const int _ON  = 0;
	static const int _OFF  = 1;
	static const int _ERROR = 2;
	
	//Information
	double m_Current_Loss[Port_Nums];

	bool TestFunction(); //-----Test
	USHORT InitFunction(const char *IniFilePath);//////////////////////////////////////////////////////////////////////////
	USHORT ConfingFileParser();
    USHORT LoadPortDifference(QString szFilePath);
    USHORT LoadCableLoss(QString szFilePath);
	USHORT NetConnect(bool bIsConnect);
	USHORT UploadWaveform(const char *WaveformPath,bool bIsOverLoad=false);
	const char* GetLastErrorMessage();
	void SetCurrentErrorMessage(USHORT usErrorCode);
	USHORT PresetFunc();
	bool SaveVsaData(const string *strTableName);
	//////////////////////////////////////////////////////////////////////////
	
	bool SortLossPointTable(vector<LossPoint>& LossPointTable);

	bool FindFreqLoss(double fFreq,const vector<LossPoint>& LossPointTable,LossPoint& loss_point);
	bool SetFreqPointTxOutputLevelAndPortAtt(const double fOutputLevel);
	bool GetCurrentFreqLoss(const double fFreq,const unsigned int uiPortIndex,double& fTxLoss,double& fRxLoss);
	void GetDutCurrentTxLoss(double fFreq,double& fTxLoss);
	
	void CurrentFilePath(char * chFileDir);

	bool ConnectSever(const char * host, const char * port = "23",unsigned int uiTimeout=15);
	void CloseServer();
	//CableLoss
	bool SetDistributionModeOnOff(bool bOnOff);
	void GetDistributionModeOnOff(bool &bOnOff);

	bool SetCurrentTableName(unsigned int uiPortIndex,bool bIsTx);
 	void GetCurrentSetTableName(string &strTableName);
 
 	bool SetCurrentLossPoint(const double fFreqMHz,const double fLoss);
 	bool GetCurrentFreqLoss(const double fFreqMHz,double &fLoss);
 
 	bool ClearCurrentAllLossData();//Clear Memory Loss Data 
 	USHORT SetAllPortLossFile(bool bIsClear);
	

	bool SaveCurrentLossFileToInstr();
	bool DeleteLossFileInInstr(unsigned int uiPortIndex,bool bIsTx);
	USHORT ResetInstrCableLossSetting();
	//-------------------------------------------------------------------------------	
	
	bool SetCenterFrequency(const double fFreqMHz);
	void GetCenterFrequency(double &fFreqMHz); 

	bool SetFreqSpan(const int iFreqSpanMHz);
	void GetFreqSpan(int &iFreqSpanMHz);

	bool SetRxRBW(const RBW uiEnum);
	void GetRxRBW(UINT64 &uiRBW_Hz);
	bool SetVsaTriggerType(const MP5800_VSA_Trigger_Type uiEnum);
	bool PowerReferenceAuto();

	bool SetRxReferenceLevel(const double fRefLevel);
	void GetRxReferenceLevel(double &fRefLevel);
	
	bool SetVsaCaptureLength(const double dCaptureLength_ms);
	bool SetVsaTriggerTimeOut(const double dTriggerTimeOut_ms);
	bool SetVsaGapTime(const double dGapTime_us);
	void GetPeakData(double &fPeakFreq,double &fPeakPower);

	
	
	bool SetVsaRx_Port(const unsigned int uiPortIndex); // Vaule:1~4
	bool Set_4Port_Switch(const PORT_MODE uiPORT_1,const PORT_MODE uiPort_2,const PORT_MODE uiPort_3,const PORT_MODE uiPort_4);
	
	//---------------------------------------------------------------
	bool SetTxOutputLevel(const double fOutputLevel);
	void GetTxOutputLevel(double &fOutputLevel);

	bool SetCWFrequencyOffset(const double fOffsetMHz);
	void GetCWFrequencyOffset(double &fOffsetMHz);

	bool LoadPlayFile(const char* strFileName);
	bool LoadPlayFileWithFilePath(const char* strFileName, const char* strFilePathName);
	
	bool SetVsgRfOnOff(const bool bRfOn);
	void GetVsgRfOnOff(bool &bRfOn);

	bool SetVsgModulationOnOff(const bool bModOn);
	void GetVsgModulationOnOff(bool &bModOn);

	bool TxPlayRepeatOnOff(const bool bOnOff);
	bool SetVsgPlayCount(const int count);
	bool SetVsgPlayGapUs(const UINT GapUs);
	bool SetVsaTriggerOffsetUs(const int OffsetUs);

	void GetChannelPower(double &fChannelPower);
	
	bool SetVsgOnOff(const bool bOnOff);
//	void GetVsgOnOff(bool &bOnOff);

	bool SetVsaOnOff(const bool bOnOff);

	bool SetMark(const double fMarkFreqMHz);
	void GetCWChannelPower(const double fMarkFreqMHz, const double leftSpan_Hz, const double rightSpan_Hz, 
	                 double &PowerMeasured, double &PowerMeasured_Left, double &PowerMeasured_Right, double &Power_Comp, double &Frequency_Offset ); 
  // the SCPI parameter: signal frequency, left span, right span
	 
	void GetMarkValue(double &fMarkValue);
	void GetPowerByTime(double &dPowerByTime);
	void GetPeakPowerFrequencyHzByTime(double &dPeakPowerFrequencyHzByTime);
	bool ClearMark(void);
// Digital down convertor
    bool SetDigitalDownConvertor(const bool bOnOff,  const double offSet_Hz );

    void GetChannelPower2Span(const double fMarkFreqMHz, const double leftSpan_Hz, const double rightSpan_Hz, 
	                     double &PowerMeasured, double &PowerMeasured_Left, double &PowerMeasured_Right );



	//FSK
	bool SetDemodulationtype(USHORT usType);
//	bool FskSetType(USHORT usType=2);
	bool FskSetType(USHORT iFskType);
	//bool FskSetSymbolPhaseContinuity(bool bIsContinue);
	bool FskSetSymbolPhaseContinuity(USHORT usCountinuity);
//	bool FskSetSymbolRate(USHORT usMHz=1);
	bool FskSetSymbolRate(double SymbolRateMHz);
	bool FSKSetFreqDeviation(double dFreqDev_kHz);
	void FSK_GetFreqDeviation(double &fFreqDevMHz);
	bool FskSetFilterType(USHORT usFilterType);
	bool FskSetFilterLength(USHORT usLength);
	bool FskSetcoefficient(double fCofficient);
	bool SetDemodulationTool(USHORT usSloution);

	//-----------------------------------------
	void GetGFSK_FreqDeviation(double &fFreqDevMHz);
	void MeasureGFSK(double &dFreqDevMHz,double &dFreqOffset,double &dFreqDrift,double &dDevError,double &dFskError);
	// set, get spectrum mode

	bool SetSpectrumOnOff(const bool bModOn);
	void GetSpectrumOnOff(bool &bModOn);

	// set, get cable loss table mode
	bool SetCableLossTableCoupleMode(const bool bModOn);
	void GetCableLossTableCoupleMode(bool &bModOn);


	// set, get spectrum mode

	bool SetMeasureMode(const Mea_Mode Mode);
	void GetMeasureMode(Mea_Mode &Mode);

	bool SetFilterWindow(const UINT filterType);

	// wifi set ---------------------


    UINT Set_WIFI_RX(   const unsigned int uiPortIndex ,                      \
	                    UINT                           frequency_MHz ,       \
					    double                    referenceLevel_dBm  ,      \
					    double                      captureLength_ms  ,      \
	                    WIFI_STANDARD                wifiStandard   ,        \
					    WIFI_BANDWIDTH            wifiBandwidth  ,       \
					    WIFI_DATA_RATE                 wifiDataRate  ,       \
					    bool                             isBeamforming );


  UINT Set_WIFI_BT_RX_Advance(   WIFI_BT_TRIGGER_TYPE               wifiBT_TriggerType            , \
	                        double                             wifiBT_TriggerGapTime_ms                   , \
							double                             wifiBT_TriggerThreshold                    , \
							double                             wifiBT_TriggerOffset                  ,  \
							WIFI_FREQUENCY_CORRECTION         wifiFrequencyCorrection              , \
							WIFI_CHANNEL_ESTIMATION           wifiChannelEstimation    );
	
 /*
  UINT Get_WIFI_RX ( double         &TXPowerdBm,               \
					double         &EVM,                      \
					bool           &MASK,                     \

					double           &PhaseError,              \
					double           &FrequencyError,          \
					double           &SymbolClockError,        \
					double           &LoLeakage,	          \
					double			&MaskTotalCount,          \
					double			&MaskErrorCount,          \
	
					bool             &isBFON,                  \
					double           &PreamblePower            );
*/
 UINT Get_WIFI_RX ( double         &TXPowerdBm,               \
					double         &EVM,                      \
					bool           &MASK,                     \

					double           &PhaseError,              \
					double           &FrequencyError,          \
					double           &SymbolClockError,        \
					double           &LoLeakage );


 UINT Set_WIFI_TX(    const unsigned int uiPortIndex ,               \
							const UINT                         frequency_MHz ,            \
							const double                         outPower_dBm,      \
							const UINT                           packetCount ,            \
							 const WIFI_STANDARD                wifiStandard,               \
						const WIFI_BANDWIDTH       wifiBandwidth,             \
						const WIFI_DATA_RATE                 wifiDataRate            );


 UINT Set_WIFI_TX_Start(void);


// ---------------------------  BT API --------------------------------------------

 UINT Set_BT_RX  ( const unsigned int uiPortIndex ,                          \
		                            UINT              frequency_MHz ,         \
					                double             referenceLevel_dBm  ,  \
					                double             captureLength_ms ,     \
	                                BT_PACKET_TYPE             BT_PacketType          );

	  
UINT Get_BT_RX(  double         &targetPower,                     \
				double         &initialFrequencyError,           \
				double         &BDR_BW_20db,                     \
				double         &BDR_FreqDrift,                   \
				double         &BDR_DeltaF1Avg,                  \
				double         &BDR_DeltaF2Avg,                  \
				double         &BDR_DeltaF2Max,	                 \
				double		   &EDR_DEVM,                        \
				double		   &EDR_DEVMPeak,                     \
				double         &LE_DeltaF1Avg,                   \
				double         &LE_DeltaF2Avg,                   \
				double         &LE_DeltaF2Max       );
	 

 UINT Set_BT_TX ( const unsigned int uiPortIndex ,                         \
		            const UINT                    frequency_MHz ,          \
					const double                  outPower_dBm   ,         \
					const double                  gapTime   ,              \
					const  UINT			        bitCount ,                  \
					const BT_PACKET_TYPE           btPacketType   );

  UINT Set_BT_TX_Start(void);

  };
#endif
