// define MP5k class
#ifndef _MP5K_H_
#define _MP5K_H_
#include <QLibrary>
/*
#ifdef _WINDLL
	#define ADIVIC_TCLIENT_API __declspec(dllexport)
#else
	#define ADIVIC_TCLIENT_API __declspec(dllimport)
#endif
*/
#include "mp5k_define.h"

typedef UINT (*mp5k_init)				(void);
typedef UINT (*mp5k_add)				(const char*, UINT&);
typedef UINT (*mp5k_delete)				(const UINT& );
typedef UINT (*mp5k_trigger_setting)	(const UINT&,const MP5K_TriggerConfig&);
typedef UINT (*mp5k_correction_setting)	(const UINT&,const MP5K_CorrectionConfig&);
typedef UINT (*mp5k_correction_setting)	(const UINT&,const MP5K_CorrectionConfig&);
typedef UINT (*mp5k_xtaltrim_setting)	(const UINT&,const MP5K_XtalTrimConfig&);
typedef UINT (*mp5k_xtaltrim_getoffset)	(const UINT&, double&);
typedef UINT (*mp5k_cal_setting)		(const UINT&,const MP5K_CalConfig&);
typedef UINT (*mp5k_cal_meas)			(const UINT&,double&);
typedef UINT (*mp5k_tx_setting)			(const UINT&,const MP5K_TxTestConfig &);
typedef UINT (*mp5k_tx_meas)			(const UINT&, MP5K_TxTestMeas&);
typedef UINT (*mp5k_tx_meas_extend)	    (const UINT& handle , MP5K_TxTestMeasExtend& txmeasext );
typedef UINT (*mp5k_rx_setting)			(const UINT&,const MP5K_RxTestConfig&);
typedef UINT (*mp5k_rx_start)			(const UINT&);
typedef UINT (*mp5k_BT_tx_setting)		( const UINT& , const MP5K_BT_TxTestConfig& );
typedef UINT (*mp5k_BT_tx_meas)			( const UINT&, MP5K_BT_TxTestMeas& );
typedef UINT (*mp5k_BT_tx_meas_extend)	( const UINT&, MP5K_BT_TxTestMeasExtend& );
typedef UINT (*mp5k_BT_rx_setting)		( const UINT& , const MP5K_BT_RxTestConfig& );
typedef UINT (*mp5k_BT_rx_start)		( const UINT& );
typedef UINT (*mp5k_close)              (void);
typedef UINT (*mp5k_VSA_agc)			( const UINT& );
typedef UINT (*mp5k_GetSpectrumPower)	( const UINT& , double& , double& , std::vector<float>& );
typedef UINT (*mp5k_VSA_SaveWaveform)	( const UINT& , const std::string& );
typedef UINT (*mp5k_VSA_LoadWaveform)	( const UINT& , const std::string& );
typedef UINT (*mp5k_RxStartMode)		( const UINT& );
typedef UINT (*mp5k_Waveform_Upload)	( const UINT& , const std::string& );
typedef UINT (*mp5k_Delete_File)		( const UINT& , const std::string& );
typedef UINT (*mp5k_GPS_init)			( void );
typedef UINT (*mp5k_GPS_add)			( const char*, UINT&  );
typedef UINT (*mp5k_GPS_delete)			( const UINT& handle );
typedef UINT (*mp5k_GPS_close)			( const UINT& handle );
typedef UINT (*mp5k_GPS_SingleCh_Setting)( const UINT& handle ,const GPS_SingleCh_Config& Setting);
typedef UINT (*mp5k_GPS_SingleCh_Start)	( const UINT& handle );
typedef UINT (*mp5k_GPS_SingleCh_Stop)	( const UINT& handle );
typedef UINT (*mp5k_GPS_CW_Signal)		( const UINT& handle );


typedef struct{
    QLibrary             mp5kLib;
    mp5k_init               mp5kInit;
    mp5k_add                mp5kAdd;
    mp5k_delete             mp5kDelete;
	mp5k_trigger_setting    mp5kTriggerSetting;
	mp5k_correction_setting mp5kCorrectionSetting;
    mp5k_xtaltrim_setting   mp5kXtalTrimSetting;
    mp5k_xtaltrim_getoffset mp5kXtalTrimGetOffset;
    mp5k_cal_setting        mp5kCalSetting;
    mp5k_cal_meas           mp5kCalMeas;    
    mp5k_tx_setting         mp5kTXSetting;
    mp5k_tx_meas            mp5kTXMeas;
	mp5k_tx_meas_extend		mp5kTxMeasExtend;
	mp5k_rx_setting         mp5kRXSetting;
	mp5k_rx_start           mp5kRXStart;
	mp5k_BT_tx_setting		mp5kBTtxsetting;
	mp5k_BT_tx_meas			mp5kBTtxmeas;
	mp5k_BT_tx_meas_extend	mp5kBTtxmeasExt;
	mp5k_BT_rx_setting		mp5kBTrxsetting;
	mp5k_BT_rx_start		mp5kBTrxstart;
	mp5k_close              mp5kClose;
	mp5k_VSA_agc			mp5kVSAagc;
	mp5k_GetSpectrumPower	mp5kGetSpectrumPower;
	mp5k_VSA_SaveWaveform	mp5kVSASaveWaveform;
	mp5k_VSA_LoadWaveform	mp5kVSALoadWaveform;
	mp5k_RxStartMode		mp5kRxStartMode;
	mp5k_Waveform_Upload	mp5kWaveform_Upload;
	mp5k_Delete_File		mp5kDeleteFile;
	
	mp5k_GPS_init				mp5kGPSinit;
	mp5k_GPS_add				mp5kGPSadd;
	mp5k_GPS_delete				mp5kGPSdelete;
	mp5k_GPS_close				mp5kGPSclose;
	mp5k_GPS_SingleCh_Setting	mp5kGPSSingleCh_Setting;
	mp5k_GPS_SingleCh_Start		mp5kGPSSingleCh_Start;
	mp5k_GPS_SingleCh_Stop		mp5kGPSSingleCh_Stop;
	mp5k_GPS_CW_Signal			mp5kGPSCW_Signal;

}mp5k_api;

class ADIVIC_TCLIENT_API mp5k
{
private:
	mp5k_api api;
public:
	mp5k(void);
	~mp5k(void);

	bool LoadDll();
	void FreeDll();

	// MP5K members
	UINT Init(void);
	UINT Add(const char*, UINT& );
	UINT Delete(const UINT&);
	UINT TriggerSetting(const UINT&, const MP5K_TriggerConfig&);
	UINT CorrectionSetting(const UINT&, const MP5K_CorrectionConfig&);
	UINT XtalTrimSetting(const UINT&, const MP5K_XtalTrimConfig&);
	UINT XtalTrimGetOffset(const UINT&, double&);
	UINT CalSetting(const UINT&,const MP5K_CalConfig&);
	UINT CalMeas(const UINT&,double&);
	UINT TXSetting(const UINT&,const MP5K_TxTestConfig &);
	UINT TXMeas(const UINT&, MP5K_TxTestMeas&);
	UINT TXMeasExtend(const UINT&, MP5K_TxTestMeasExtend&);
	UINT RXSetting(const UINT&,const MP5K_RxTestConfig&);
	UINT RXStart( const UINT&);	
	UINT BT_Tx_Setting( const UINT& , const MP5K_BT_TxTestConfig& );
	UINT BT_Tx_Meas( const UINT&, MP5K_BT_TxTestMeas&  );
	UINT BT_Tx_MeasExtend( const UINT&, MP5K_BT_TxTestMeasExtend&  );
	UINT BT_Rx_Setting( const UINT& , const MP5K_BT_RxTestConfig& );
	UINT BT_Rx_Start( const UINT& );
	UINT Close( void );


	UINT GPS_init( void );
	UINT GPS_add( const char*, UINT&  );
	UINT GPS_delete( const UINT&  );
	UINT GPS_close( const UINT&  );
	UINT GPS_SingleCh_Setting( const UINT&  ,const GPS_SingleCh_Config&);
	UINT GPS_SingleCh_Start( const UINT&  );
	UINT GPS_SingleCh_Stop( const UINT&  );
	UINT GPS_CW_Signal( const UINT&  );

};


#endif
