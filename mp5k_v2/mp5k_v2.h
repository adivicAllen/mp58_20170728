/*------------------------------------------*/
/*			ADIVIC Tecknology Co.		    */
/*	ADIVIC Wireless Test Station MP5000		*/
/*			MP5K.dll (V 1.0.0.1				*/
/*											*/
/*				2016/11/30					*/
/*------------------------------------------*/

#define MULTIDUT	1
#define GEMTEKS		1
#define GPSAPI		1


#include "TOP_LEVEL.h"
#include "mp5k_define.h"
// MP5K EXPORT

 
// MP5K EXPORT

#if MP5800API
  #define FOURPORT_CHECK 0
#endif

#if MP5000API
  #define FOURPORT_CHECK 1
#endif
MP5000_API UINT mp5k_init( void );

MP5000_API UINT mp5k_add( const char*, UINT& );
MP5000_API UINT mp5k_delete( const UINT& );

MP5000_API UINT mp5k_trigger_setting( const UINT& , const MP5K_TriggerConfig& );
MP5000_API UINT mp5k_correction_setting( const UINT& , const MP5K_CorrectionConfig& );

MP5000_API UINT mp5k_xtaltrim_setting( const UINT& , const MP5K_XtalTrimConfig& );
MP5000_API UINT mp5k_xtaltrim_getoffset( const UINT& , double& );

MP5000_API UINT mp5k_cal_setting( const UINT& , const MP5K_CalConfig& );
MP5000_API UINT mp5k_cal_meas( const UINT& , double& );

MP5000_API UINT mp5k_tx_setting( const UINT& , const MP5K_TxTestConfig& );
MP5000_API UINT mp5k_tx_meas( const UINT&, MP5K_TxTestMeas& );
MP5000_API UINT mp5k_tx_meas_extend( const UINT& handle , MP5K_TxTestMeasExtend& txmeasext );

MP5000_API UINT mp5k_rx_setting( const UINT& , const MP5K_RxTestConfig& );
MP5000_API UINT mp5k_rx_start( const UINT& );

MP5000_API UINT mp5k_get_error_message( const UINT& , std::string& );
MP5000_API UINT mp5k_close( const UINT& ); //


//BlueTooth
MP5000_API UINT mp5k_BT_tx_setting( const UINT& , const MP5K_BT_TxTestConfig& );
MP5000_API UINT mp5k_BT_tx_meas( const UINT&, MP5K_BT_TxTestMeas& );
MP5000_API UINT mp5k_BT_tx_meas_extend( const UINT&, MP5K_BT_TxTestMeasExtend& );
MP5000_API UINT mp5k_BT_rx_setting( const UINT& , const MP5K_BT_RxTestConfig& );
MP5000_API UINT mp5k_BT_rx_start( const UINT& );

//Cableloss
MP5000_API UINT mp5k_Cableloss_init( const UINT&  );
MP5000_API UINT mp5k_Cableloss_Meas( const UINT& , MP5K_Cableloss_Data& );
MP5000_API UINT mp5k_Cableloss_close( const UINT& );

// Multi
#if MULTIDUT
MP5000_API UINT mp5k_multi_dut( const bool& );                                    // MULTI DUT STATUS [Enable]
MP5000_API UINT mp5k_change_id( const UINT& , const UINT& );                      // CHANGE ID [MyID,NextID]
MP5000_API UINT mp5k_wait_server_ready( const UINT& , const UINT& );              // WAIT SERVER READY
MP5000_API UINT mp5k_release_lock( const UINT& );                                 // RELEASE SERVER LOCK
MP5000_API UINT mp5k_measure_done( const UINT& );                                 // MEASURE DONE
MP5000_API UINT mp5k_watchdog_stop( const UINT& );
#endif


//GemTeks
#if GEMTEKS
MP5000_API UINT mp5k_VSA_AGC( const UINT& );
MP5000_API UINT mp5k_GetSpectrumPower( const UINT& , double& , double& , std::vector<float>& );
MP5000_API UINT mp5k_VSA_SaveWaveform(const UINT& handle, const std::string& filename);
MP5000_API UINT mp5k_VSA_LoadWaveform(const UINT& handle, const std::string& filename);
MP5000_API UINT mp5k_RxStartMode( const MP5K_RX_Start_Mode& );
MP5000_API UINT mp5k_waveform_Upload( const UINT& , const std::string& );
MP5000_API UINT mp5k_Delete_File( const UINT& , const std::string& );
#endif

//GPS
#if GPSAPI
MP5000_API UINT mp5k_GPS_init( void );
MP5000_API UINT mp5k_GPS_add( const char*, UINT&  );
MP5000_API UINT mp5k_GPS_delete( const UINT& handle );
MP5000_API UINT mp5k_GPS_close( const UINT& handle );
MP5000_API UINT mp5k_GPS_SingleCh_Setting( const UINT& handle ,const GPS_SingleCh_Config& Setting);
MP5000_API UINT mp5k_GPS_SingleCh_Start( const UINT& handle );
MP5000_API UINT mp5k_GPS_SingleCh_Stop( const UINT& handle );
MP5000_API UINT mp5k_GPS_CW_Signal( const UINT& handle );
#endif

#ifndef MP5800
MP5000_API UINT mp5k_Send_Cmd( const UINT& handle, std::string& cmd);
MP5000_API UINT mp5k_Read_Cmd( const UINT& handle, std::string& data, const std::string& token);
#endif
