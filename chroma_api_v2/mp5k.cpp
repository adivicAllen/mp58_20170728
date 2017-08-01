#include "stdafx.h"
#include "mp5k.h"


mp5k::mp5k(void)
{
	memset( &api, 0, sizeof(api));
}


mp5k::~mp5k(void)
{
}

bool mp5k::LoadDll(void)
{
	api.mp5kLib = LoadLibrary(L"mp5k.dll");
	if (!api.mp5kLib)
	{
		return false;
	}
	
	HINSTANCE Mp5kLib = api.mp5kLib;
	api.mp5kInit				= ( mp5k_init)GetProcAddress(Mp5kLib,"mp5k_init");
	api.mp5kAdd					= ( mp5k_add)GetProcAddress(Mp5kLib,"mp5k_add");
	api.mp5kDelete				= ( mp5k_delete)GetProcAddress(Mp5kLib,"mp5k_delete");
	api.mp5kTriggerSetting		= ( mp5k_trigger_setting)GetProcAddress(Mp5kLib,"mp5k_trigger_setting");
	api.mp5kCorrectionSetting	= ( mp5k_correction_setting)GetProcAddress(Mp5kLib,"mp5k_correction_setting");
	api.mp5kXtalTrimSetting		= ( mp5k_xtaltrim_setting)GetProcAddress(Mp5kLib,"mp5k_xtaltrim_setting");
	api.mp5kXtalTrimGetOffset	= ( mp5k_xtaltrim_getoffset)GetProcAddress(Mp5kLib,"mp5k_xtaltrim_getoffset");
	api.mp5kCalSetting			= ( mp5k_cal_setting)GetProcAddress(Mp5kLib,"mp5k_cal_setting");
	api.mp5kCalMeas				= ( mp5k_cal_meas)GetProcAddress(Mp5kLib,"mp5k_cal_meas");
	api.mp5kTXSetting			= ( mp5k_tx_setting)GetProcAddress(Mp5kLib,"mp5k_tx_setting");
	api.mp5kTXMeas				= ( mp5k_tx_meas)GetProcAddress(Mp5kLib,"mp5k_tx_meas");
	api.mp5kTxMeasExtend		= ( mp5k_tx_meas_extend )GetProcAddress(Mp5kLib,"mp5k_tx_meas_extend");
	api.mp5kRXSetting			= ( mp5k_rx_setting)GetProcAddress(Mp5kLib,"mp5k_rx_setting");
	api.mp5kRXStart				= ( mp5k_rx_start)GetProcAddress(Mp5kLib,"mp5k_rx_start");
	api.mp5kBTtxsetting			= ( mp5k_BT_tx_setting )GetProcAddress(Mp5kLib,"mp5k_BT_tx_setting");
	api.mp5kBTtxmeas			= ( mp5k_BT_tx_meas )GetProcAddress(Mp5kLib,"mp5k_BT_tx_meas");
	api.mp5kBTtxmeasExt			= ( mp5k_BT_tx_meas_extend )GetProcAddress(Mp5kLib, "mp5k_BT_tx_meas_extend");
	api.mp5kBTrxsetting			= ( mp5k_BT_rx_setting )GetProcAddress(Mp5kLib,"mp5k_BT_rx_setting");
	api.mp5kBTrxstart			= ( mp5k_BT_rx_start )GetProcAddress(Mp5kLib,"mp5k_BT_rx_start");
	api.mp5kClose				= ( mp5k_close )GetProcAddress(Mp5kLib,"mp5k_close");

	api.mp5kGPSinit				= ( mp5k_GPS_init )GetProcAddress(Mp5kLib,"mp5k_GPS_init");
	api.mp5kGPSadd				= ( mp5k_GPS_add )GetProcAddress(Mp5kLib,"mp5k_GPS_add");
	api.mp5kGPSdelete			= ( mp5k_GPS_delete )GetProcAddress(Mp5kLib,"mp5k_GPS_delete");
	api.mp5kGPSclose			= ( mp5k_GPS_close )GetProcAddress(Mp5kLib,"mp5k_GPS_close");
	api.mp5kGPSSingleCh_Setting	= ( mp5k_GPS_SingleCh_Setting )GetProcAddress(Mp5kLib,"mp5k_GPS_SingleCh_Setting");
	api.mp5kGPSSingleCh_Start	= ( mp5k_GPS_SingleCh_Start )GetProcAddress(Mp5kLib,"mp5k_GPS_SingleCh_Start");
	api.mp5kGPSSingleCh_Stop	= ( mp5k_GPS_SingleCh_Stop )GetProcAddress(Mp5kLib,"mp5k_GPS_SingleCh_Stop");
	api.mp5kGPSCW_Signal		= ( mp5k_GPS_CW_Signal )GetProcAddress(Mp5kLib,"mp5k_GPS_CW_Signal");
	
	if (
		( api.mp5kInit				== NULL) ||
		( api.mp5kAdd				== NULL) ||
		( api.mp5kDelete			== NULL) ||
		( api.mp5kTriggerSetting	== NULL) ||
		( api.mp5kCorrectionSetting == NULL) ||
		( api.mp5kXtalTrimSetting	== NULL) ||
		( api.mp5kXtalTrimGetOffset == NULL) ||
		( api.mp5kCalSetting		== NULL) ||
		( api.mp5kCalMeas			== NULL) ||
		( api.mp5kTXSetting			== NULL) ||
		( api.mp5kTXMeas			== NULL) ||
		( api.mp5kRXSetting			== NULL) ||
		( api.mp5kTxMeasExtend		== NULL) ||
		( api.mp5kClose				== NULL) ||
		( api.mp5kGPSinit			== NULL) ||		
		( api.mp5kGPSadd			== NULL) ||		
		( api.mp5kGPSdelete			== NULL) ||	
		( api.mp5kGPSclose			== NULL) ||	
		( api.mp5kGPSSingleCh_Setting	== NULL) ||
		( api.mp5kGPSSingleCh_Start		== NULL) ||
		( api.mp5kGPSSingleCh_Stop		== NULL) ||
		( api.mp5kGPSCW_Signal			== NULL) 		
		)
	{
		FreeLibrary(Mp5kLib);
		return false;
	}else
		return true;
}

void mp5k::FreeDll()
{
	if (api.mp5kLib != NULL)
	{
		bool tol = FreeLibrary(api.mp5kLib);
	
	api.mp5kLib = NULL;
	}
}

UINT mp5k::Init()
{
	return (*api.mp5kInit)();
}

UINT mp5k::Add(const char* ip, UINT& handle)
{
	return (*api.mp5kAdd)(ip,handle);
}

UINT mp5k::Delete(const UINT& handle)
{
	return (*api.mp5kDelete)(handle);
}

UINT mp5k::TriggerSetting(const UINT& handle, const MP5K_TriggerConfig& trigger)
{
	return (*api.mp5kTriggerSetting)(handle,trigger);
}

UINT mp5k::CorrectionSetting(const UINT& handle, const MP5K_CorrectionConfig& corr)
{
	return (*api.mp5kCorrectionSetting)(handle,corr);
}

UINT mp5k::XtalTrimSetting(const UINT& handle,const MP5K_XtalTrimConfig& cfg)
{
	return (*api.mp5kXtalTrimSetting)(handle,cfg);
}

UINT mp5k::XtalTrimGetOffset(const UINT& handle, double& offsetHz)
{
	return (*api.mp5kXtalTrimGetOffset)(handle,offsetHz);
}

UINT mp5k::CalSetting(const UINT& handle,const MP5K_CalConfig& cfg)
{
	return (*api.mp5kCalSetting)(handle,cfg);
}

UINT mp5k::CalMeas(const UINT& handle,double& powerdBm)
{
	return (*api.mp5kCalMeas)(handle,powerdBm);
}

UINT mp5k::TXSetting(const UINT& handle,const MP5K_TxTestConfig & cfg)
{
	return (*api.mp5kTXSetting)(handle,cfg);
}

UINT mp5k::TXMeas(const UINT& handle, MP5K_TxTestMeas& meas)
{
	return (*api.mp5kTXMeas)(handle,meas);
}

UINT mp5k::TXMeasExtend( const UINT& handle , MP5K_TxTestMeasExtend& txmeasext )
{
	return(*api.mp5kTxMeasExtend)(handle,txmeasext);
}

UINT mp5k::RXSetting(const UINT& handle,const MP5K_RxTestConfig& cfg)
{
	return (*api.mp5kRXSetting)(handle,cfg);
}

UINT mp5k::RXStart(const UINT& handle)
{
	return (*api.mp5kRXStart)(handle);
}

UINT mp5k::BT_Tx_Setting( const UINT& handle , const MP5K_BT_TxTestConfig& bt_tx_cfg )
{
	return(*api.mp5kBTtxsetting)(handle,bt_tx_cfg);
}

UINT mp5k::BT_Tx_Meas( const UINT& handle , MP5K_BT_TxTestMeas& bt_txmeas )
{
	return(*api.mp5kBTtxmeas)(handle,bt_txmeas);
}

UINT mp5k::BT_Tx_MeasExtend( const UINT& handle , MP5K_BT_TxTestMeasExtend& bt_txmeasExt)
{
	return(*api.mp5kBTtxmeasExt)(handle, bt_txmeasExt);
}

UINT mp5k::BT_Rx_Setting( const UINT& handle , const MP5K_BT_RxTestConfig& bt_rx_cfg )
{
	return(*api.mp5kBTrxsetting)(handle,bt_rx_cfg);
}

UINT mp5k::BT_Rx_Start( const UINT& handle  )
{
	return(*api.mp5kBTrxstart)(handle);
}

UINT mp5k::Close( void )
{
	return ( *api.mp5kClose )();
}

UINT mp5k::GPS_init( void )
{
	return(*api.mp5kGPSinit)();
}

UINT mp5k::GPS_add( const char* ip , UINT&  handle)
{
	return(*api.mp5kGPSadd)( ip, handle);
}

UINT mp5k::GPS_delete( const UINT& handle )
{
	return(*api.mp5kGPSdelete)( handle );
}

UINT mp5k::GPS_close( const UINT& handle )
{
	return(*api.mp5kGPSclose)( handle );
}

UINT mp5k::GPS_SingleCh_Setting( const UINT& handle ,const GPS_SingleCh_Config& Setting)
{
	return(*api.mp5kGPSSingleCh_Setting)( handle , Setting );
}

UINT mp5k::GPS_SingleCh_Start( const UINT& handle )
{
	return(*api.mp5kGPSSingleCh_Start)( handle );
}

UINT mp5k::GPS_SingleCh_Stop( const UINT& handle )
{
	return(*api.mp5kGPSSingleCh_Stop)( handle );
}

UINT mp5k::GPS_CW_Signal( const UINT& handle )
{
	return(*api.mp5kGPSCW_Signal)( handle );
}




















