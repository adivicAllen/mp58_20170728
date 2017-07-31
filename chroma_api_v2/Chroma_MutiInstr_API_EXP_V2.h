#pragma once

#ifdef _CHROMA_API_EXPORTS_LIB
#define CHROMA_API_DLLEXT  extern "C"  __declspec(dllexport)
#else
#define CHROMA_API_DLLEXT  extern "C" __declspec(dllimport)
#endif

#include "MP5800_API_define.h"
using namespace MP5800;
/****************************************************
** wifi BT enum
**************************************************** */

enum WIFI_BT_TRIGGER_TYPE
{
	WIFI_BT_TRIGGER_IMMEDIATE,
	WIFI_BT_TRIGGER_POSTIVE,
	WIFI_BT_TRIGGER_NEGATIVE,
	WIFI_BT_TRIGGER_LEVEL,
	WIFI_BT_TRIGGER_EXTERNAL,
};

enum RBW
{
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
};
 


enum SAMPLE_RATE
{
	SAMPLE_RATE_5_MHz=0,
	SAMPLE_RATE_10_MHz,
	SAMPLE_RATE_20_MHz,
	SAMPLE_RATE_40_MHz,
	SAMPLE_RATE_80_MHz,
	SAMPLE_RATE_160_MHz
};

enum VSA_TRIGGER_TYPE
{
	VSA_TRIGGER_IMMEDIATE,
	VSA_TRIGGER_POSTIVE,
	VSA_TRIGGER_NEGATIVE,
	VSA_TRIGGER_LEVEL
};
 
 

// system function for TCP?IP
CHROMA_API_DLLEXT USHORT __stdcall expMV2_CloseAllServer();
CHROMA_API_DLLEXT USHORT __stdcall expMV2_CloseServer(RF_PORT PortIndex);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_ConnectSever(RF_PORT PortIndex,const char * HostAddress, const char * Port,unsigned int uiTimeout=15);
CHROMA_API_DLLEXT const char* __stdcall expMV2_GetLastErrorMessage();
CHROMA_API_DLLEXT USHORT __stdcall expMV2_InitFunction(const char *IniFilePath);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetDisplayOnOffByConnection( bool bDebugMode);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetSeverIsConnect(RF_PORT PortIndex,bool &bIsConnect);//
CHROMA_API_DLLEXT USHORT __stdcall expMV2_UploadWaveform(const char *WaveformFilePath);//
CHROMA_API_DLLEXT USHORT __stdcall expMV2_PresetServer(RF_PORT PortIndex);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_OpenServer();//


// MP5800 port setting, calbe loss, standard setting
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetCurrentFreqLoss(const double fFreq,const RF_PORT PortIndex,double& fTxLoss,double& fRxLoss);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_InputCableLoss(RF_PORT PortIndex,const char *InstrCableLossFilePath);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetCableLossEnable(RF_PORT PortIndex,const bool bEnable);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetCableLossTableCoupleMode(RF_PORT PortIndex, const bool bOnOff);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetDistributionModeOnOff(RF_PORT PortIndex,const bool bOnOff);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetDistributionModeOnOff(RF_PORT PortIndex,bool &bDistributionMode);


// MP5800 port setting, calbe loss, standard setting
CHROMA_API_DLLEXT USHORT __stdcall expMV2_Set_4Port_Switch(const RF_PORT PortFirstIndex,const PORT_MODE pm_PORT_1,
																							const PORT_MODE pm_Port_2, 
																							const PORT_MODE pm_Port_3,
																							const PORT_MODE pm_Port_4);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetMeasureMode( RF_PORT PortIndex, const MP5800_STANDARD  standard );


// set vSA

CHROMA_API_DLLEXT USHORT __stdcall expMV2_PowerReferenceAuto(RF_PORT PortIndex);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetCenterFrequency(RF_PORT PortIndex,const double fFreqMHz,const double fFreqOffsetMHz=0); //50~6000 MHz
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetFilterWindow(RF_PORT PortIndex, const UINT filterType);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetFreqSpan(RF_PORT PortIndex,const int iFreqSpanMHz);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SaveVsaData(RF_PORT PortIndex,const char *SaveName);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetRxRBW(RF_PORT PortIndex,const RBW uiEnum);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetRxRBW_Int(RF_PORT PortIndex,const int iIndex);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetRxReferenceLevel(RF_PORT PortIndex,const double fRefLevel);//10~-80 dBm
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetSpectrumOnOff(RF_PORT PortIndex, const bool bOnOff);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetVsaOnOff(RF_PORT PortIndex,const bool bOnOff);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetVsaRx_Port(const RF_PORT PortIndex);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetVsaTriggerType(RF_PORT PortIndex,const MP5800_VSA_Trigger_Type uiTriggerType);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetVsaTriggerTimeOut( RF_PORT PortIndex,const double dTriggerTimeOut_ms );
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetVsaCaptureLength( RF_PORT PortIndex,const double dCapturetureLength_ms );
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetVsaGapTime(RF_PORT PortIndex,const double dGapTime_us);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetVsaTriggerOffsetUs(RF_PORT PortIndex,const int OffsetUs);
// get valuse of VSA 
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetCenterFrequency(RF_PORT PortIndex,double &fFreqMHz);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetFreqSpan(RF_PORT PortIndex,int &iFreqSpanMHz);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetRxRBW(RF_PORT PortIndex,UINT64 &uiRBW_Hz);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetRxReferenceLevel(RF_PORT PortIndex,double &fRefLevel);



// vsa measure power API
CHROMA_API_DLLEXT USHORT __stdcall expMV2_ClearMark(RF_PORT PortIndex);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetChannelPower(RF_PORT PortIndex,double &fChannelPower);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetChannelPower2Span(RF_PORT PortIndex,const double fMarkFreqMHz, const double leftSpan_Hz, const double rightSpan_Hz, 
	double &PowerMeasured, double &PowerMeasured_Left, double &PowerMeasured_Right );
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetCWChannelPower(RF_PORT PortIndex,const double fMarkFreqMHz, const double leftSpan_Hz, const double rightSpan_Hz, 
	double &PowerMeasured, double &PowerMeasured_Left, double &PowerMeasured_Right, double &Power_Comp, double &Frequency_Offset );
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetMarkValue(RF_PORT PortIndex,double &fMarkValue);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetPeakData(RF_PORT PortIndex,double &fPeakFreqMHz,double &fPeakPower);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetPowerByTime(RF_PORT PortIndex,double &dPowerByTime);       // 20161202
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetPeakPowerFrequencyHzByTime(RF_PORT PortIndex,double &dPeakPowerFrequencyHzByTime);  // 20161202
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetDigitalDownConvertor(RF_PORT PortIndex,const bool bOnOff, \
	                               const double offSet_Hz =1000000 );
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetMark(RF_PORT PortIndex,const double fMarkFreqMHz);


// set VSG value
CHROMA_API_DLLEXT USHORT __stdcall expMV2_LoadMemory(RF_PORT PortIndex,const char * strFileName);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_LoadMemoryWithFilePath(RF_PORT PortIndex,const char * strFileName, const char *strFilePath);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_ModulationOnOff(RF_PORT PortIndex,const bool bModulationOnOff);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_Rf_OnOff(RF_PORT PortIndex,const bool bRfOnOff);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetCWFrequencyOffset(RF_PORT PortIndex,const double fFreqOffsetMHz); //-50~50MHz
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetTxOutputLevel(RF_PORT PortIndex,const double fOutputLevel);//10~-90 dBm
CHROMA_API_DLLEXT USHORT __stdcall expMV2_TxPlayRepeatOnOff(RF_PORT PortIndex,const bool bOnOff);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetVsgPlayCount(RF_PORT PortIndex,const int count);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetVsgPlayGapUs(RF_PORT PortIndex,const UINT GapUs);


// get value from VSG
CHROMA_API_DLLEXT USHORT __stdcall expMV2_Get_Rf_OnOff(RF_PORT PortIndex,bool& bRfOnOff);
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetTxOutputLevel(RF_PORT PortIndex,double &fOutputLevel);




// modulatiion

CHROMA_API_DLLEXT USHORT __stdcall expMV2_Set_GPRF_DemodulationType( const RF_PORT PortIndex, const MP5800_GPRF_Demodulation& demodualtion);
 

CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetFskDemodulation(const RF_PORT PortIndex,   \
	                                                         const USHORT usSolution = 0,         \
											                 const USHORT usFilerType = 3,        \
															 const USHORT usCountinuity = 0,      \
															 const USHORT usFSKType = 0,          \
															 const USHORT usFilterLength = 6,     \
															 const double dBandwidthTime = 0.5,   \
															 const double dSymbolRateMHz = 1.0,   \
															 const double dFrequencyDeviation_kHz = 250);
 


CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetFSK_FreqDeviation(RF_PORT PortIndex,double &fFreqDevMHz);

CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetFSKDemodulation(RF_PORT PortIndex,double &dFreqDevMHz,double &dFreqOffset,double &dFreqDrift,double &DevError,double &FskError);
 


CHROMA_API_DLLEXT UINT __stdcall expMV2_Set_WIFI_RX( 
	                                                    const RF_PORT            PortIndex   = MP5800::RF_PORT_1   ,                  \
														const UINT                           frequency_MHz  = 5180,                   \
														const double                    referenceLevel_dBm  = -10.0,                   \
														const double                      captureLength_ms  = 0.001, 
														const WIFI_STANDARD                wifiStandard   =  _802_11n,                  \
														const WIFI_BANDWIDTH                  wifiBandwidth  = _20M,       \
														const WIFI_DATA_RATE                 wifiDataRate  = MCS7,   
														const bool                             isBeamforming = false );  


CHROMA_API_DLLEXT UINT __stdcall expMV2_Set_WIFI_BT_RX_Advance(
	                                                        const RF_PORT                    PortIndex = MP5800::RF_PORT_1  , 
	                                                        const WIFI_BT_TRIGGER_TYPE                  wifiBT_TriggerType   = WIFI_BT_TRIGGER_POSTIVE          , \
	                                                        const double                              wifiBT_TriggerGapTime_ms = 0.00001              , \
														    const double                              wifiBT_TriggerThreshold  = -20.00               , \
														    const double                              wifiBT_TriggerOffset = -0.0001                  , \
														    const WIFI_FREQUENCY_CORRECTION         wifiFrequencyCorrection = LTF_CORRECTION         , \
														    const WIFI_CHANNEL_ESTIMATION           wifiChannelEstimation = DATA_ESTIMATION  );

/*
CHROMA_API_DLLEXT UINT __stdcall expMV2_Get_WIFI_RX(  
	                                                    const RF_PORT    PortIndex    ,             \
                                                        double         &TXPowerdBm,               \
														double         &EVM,                      \
														bool           &bMASK,                     \
													   double           &PhaseError,              \
													   double           &FrequencyError,          \
													   double           &SymbolClockError,        \
													   double           &LoLeakage,	              \
													   double			&MaskTotalCount,          \
													   double			&MaskErrorCount,          \
													   bool             &isBFON,                  \
													   double           &PreamblePower            );
*/
CHROMA_API_DLLEXT UINT __stdcall expMV2_Get_WIFI_RX(  
	                                                    const RF_PORT    PortIndex    ,             \
                                                        double         &TXPowerdBm,               \
														double         &EVM,                      \
														bool           &bMASK,                     \
													   double           &PhaseError,              \
													   double           &FrequencyError,          \
													   double           &SymbolClockError,        \
													   double           &LoLeakage );
 

CHROMA_API_DLLEXT UINT  __stdcall expMV2_Set_WIFI_TX(
	                                                    const RF_PORT                 PortIndex   = MP5800::RF_PORT_1   ,          \
                                                        const UINT                    frequency_MHz  = 5180,               \
														 const double                    outPower_dBm  = -10.00  ,         \
														  const UINT                       packetCount  = 0,               \
														 const WIFI_STANDARD                wifiStandard   =  _802_11n,                  \
														const WIFI_BANDWIDTH           wifiBandwidth  = MP5800::_20M,       \
														const WIFI_DATA_RATE                 wifiDataRate  = MCS7              );


CHROMA_API_DLLEXT UINT __stdcall expMV2_Set_WIFI_TX_Start(
	                                                          const RF_PORT                 PortIndex   = MP5800::RF_PORT_1,
															  const bool                    OnOff = true);





// ------------------------------------------ BT API --------------------------------------------------

/*
enum BT_STANDARD
{
    BT_STANDARD_BDR = 0,
    BT_STANDARD_EDR,
    BT_STANDARD_LE,

} ;

enum BT_PAYLOAD_DATA
{
	BT_PAYLOAD_DATA_ALL_ZERO,
	BT_PAYLOAD_DATA_ALL_ONE,
	BT_PAYLOAD_DATA_ZOZO,
	BT_PAYLOAD_DATA_FOFO,
	BT_PAYLOAD_DATA_PN9,
	BT_PAYLOAD_DATA_PN15,
	BT_PAYLOAD_DATA_OFOF,
	BT_PAYLOAD_DATA_OZOZ

};

enum BT_PACKET_TYPE
{
	// BDR
	BT_PACKET_TYPE_BDR_DM1,
	DH1,
	BT_PACKET_TYPE_BDR_DM3,
	BT_PACKET_TYPE_BDR_DH3,
	BT_PACKET_TYPE_BDR_DM5,
	BT_PACKET_TYPE_BDR_DH5,
	// EDR
	BT_PACKET_TYPE_EDR_2DH1,
	BT_PACKET_TYPE_EDR_2DH3,
	BT_PACKET_TYPE_EDR_2DH5,
	BT_PACKET_TYPE_EDR_3DH1,
	BT_PACKET_TYPE_EDR_3DH3,
	BT_PACKET_TYPE_EDR_3DH5,
	// LE
	BT_PACKET_TYPE_LE

};
 
 */

CHROMA_API_DLLEXT UINT __stdcall expMV2_Set_BT_RX( 
	                                                    const RF_PORT                    PortIndex   = MP5800::RF_PORT_1   ,                     \
														const UINT                           frequency_MHz  = 2402,                   \
														const double                    referenceLevel_dBm  = -10.0,                   \
														const double                      captureLength_ms  = 0.001, 
														const BT_PACKET_TYPE             btPacketType   =  DH1    );  



CHROMA_API_DLLEXT UINT __stdcall expMV2_Get_BT_RX(  
	                                                    const RF_PORT  PortIndex    ,                    \
                                                        double         &targetPower,                     \
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
 

 

CHROMA_API_DLLEXT UINT  __stdcall expMV2_Set_BT_TX(
	                                                    const RF_PORT                 PortIndex   = MP5800::RF_PORT_1   ,          \
                                                        const UINT                    frequency_MHz  = 2402,               \
														 const double                  outPower_dBm  = -60.00  ,         \
														 const double                  gapTime   = 0.000625,                   \
														 const  UINT			        bitCount = 160,                   \
														 const BT_PACKET_TYPE           btPacketType   =  DH1   );

CHROMA_API_DLLEXT UINT __stdcall expMV2_Set_BT_TX_Start(
	                                                          const RF_PORT                 PortIndex   = MP5800::RF_PORT_1 ,
															  const bool  OnOff = true);

 

 
 
 		 