
#include <map>
#include <iostream>
#include <fstream> 
#include <vector>
#include <algorithm>
#include <string>
using namespace std;
#include "MP5800_API.h"
#include "Chroma_MutiInstr_API_EXP_V2.h"
typedef std::map<MEASURE_ITEM,double> MP5800_TABLE;
//#define MP5000 1
#define MP5800_PORT_NOs  4
#define MP5800_NO 2
#define COMMAND_MANAGER 1
#define BUFFER_LENGTH 513
bool bDebug = false;
bool bInit = false;
// ----------------------------------
 
 

// ------------------------------------


std::map<int,  MP5800_TABLE> mp5800Table;
std::vector<int> mp5800TableIndex;
std::map<int,std::vector<MEASURE_ITEM>> mp5800MeasureItemIndex;
std::map<BT_PACKET_TYPE, std::string> btPackageType;
std::map<BT_DATA_TYPE, std::string> btDataType;

std::map <WIFI_STANDARD, std::string>    wifiStandard;
std::map <WIFI_BANDWIDTH, std::string>  wifiChannelBandwidth;
std::map <WIFI_DATA_RATE , std::string > wifiDataRate;

 
//std::map<std::string, std::map<std::string, double> >  mp5800Table;
typedef std::map<MEASURE_ITEM, double> CUSTOMIZED_TABLE;
CUSTOMIZED_TABLE customerizedTable;
// Is the following typedef'd type inside a map a good idea?
typedef std::vector<CUSTOMIZED_TABLE> customizedTableVector;  

// -----------------------------------------------
MP5800_CONFIG       mp5800TesterConfig[MP5800_NO];
MP5800_VSG_CONFIG   mp5800VsgConfig[MP5800_NO];
	// config
MP5800_VSA_CONFIG   mp5800VsaConfig[MP5800_NO];
MP5800_FSK_CONFIG   mp5800FskConfig[MP5800_NO];
MP5800_WIFI_CONFIG  mp5800WifiConfig[MP5800_NO];
MP5800_BT_CONFIG    mp5800BtConfig[MP5800_NO];
	// 
MP5800_VSA_MEASURE   mp5800VsaMeasure[MP5800_NO];
MP5800_FSK_MEASURE   mp5800FskMeasure[MP5800_NO];
MP5800_WIFI_MEASURE  mp5800WifiMeasure[MP5800_NO];
MP5800_BT_MEASURE    mp5800BtMeasure[MP5800_NO];
bool mp5800LoopBack[MP5800_NO];
char* btLoadFileName[MP5800_NO];
MP5800_STANDARD mp5800TesterConfigStandard[MP5800_NO];
// ---------------------------------------------------
// inital
  MP5800_CONFIG       testerConfig;
 
  // config
  MP5800_VSG_CONFIG   vsgConfig;
  MP5800_VSA_CONFIG   vsaConfig;
  MP5800_FSK_CONFIG   fskConfig;
  MP5800_WIFI_CONFIG  wifiConfig;
  MP5800_BT_CONFIG    btConfig;
  // 
  MP5800_VSA_MEASURE vsaMeasure;
  MP5800_FSK_MEASURE fskMeasure;
  MP5800_WIFI_MEASURE  wifiMeasure;
  MP5800_BT_MEASURE    btMeasure;
  // --------------------------------------

UINT queryTable(UINT MP5800_getFail, int tableIndex ,MEASURE_ITEM queryItem,  double queryResult )
{
		if( MP5800_getFail)       {
				mp5800Table[tableIndex][queryItem] = MP5800_ERROR_VALUE;
				return MP5800_ERROR;
		}
		else
		{
				mp5800Table[tableIndex][queryItem] = queryResult;
		}
		return MP5800_SUCCESS;												 
}

void makeWIFI_WaveformMap()
{
	wifiStandard.insert(pair<WIFI_STANDARD,std::string> ( _802_11ac , "_802_11ac"));
	wifiStandard.insert(pair<WIFI_STANDARD,std::string> ( _802_11n , "_802_11n"));
	wifiStandard.insert(pair<WIFI_STANDARD,std::string> ( _802_11b , "_802_11b"));
	wifiStandard.insert(pair<WIFI_STANDARD,std::string> ( _802_11g , "_802_11g"));
	wifiStandard.insert(pair<WIFI_STANDARD,std::string> ( _802_11a , "_802_11a"));
 
	wifiChannelBandwidth.insert( pair<WIFI_BANDWIDTH ,std::string> ( _20M , "_20M"));
	wifiChannelBandwidth.insert( pair<WIFI_BANDWIDTH ,std::string> ( _40M , "_40M"));
	wifiChannelBandwidth.insert( pair<WIFI_BANDWIDTH ,std::string> ( _80M , "_80M"));

	  wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( CCK_1M , "CCK_1M"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( CCK_2M , "CCK_2M"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( CCK_55M , "CCK_55M"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( CCK_11M , "CCK_11M"));


	 
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( OFDM_6M , "OFDM_6M"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( OFDM_9M , "OFDM_9M"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( OFDM_12M , "OFDM_12M"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( OFDM_18M , "OFDM_18M"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( OFDM_24M , "OFDM_24M"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( OFDM_36M , "OFDM_36M"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( OFDM_48M , "OFDM_48M"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( OFDM_54M , "OFDM_54M"));

	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( MCS0 , "MCS0"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( MCS1 , "MCS1"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( MCS2 , "MCS2"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( MCS3 , "MCS3"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( MCS4 , "MCS4"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( MCS5 , "MCS5"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( MCS6 , "MCS6"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( MCS7 , "MCS7"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( MCS8 , "MCS8"));
	wifiDataRate.insert( pair< WIFI_DATA_RATE ,std::string> ( MCS9 , "MCS9"));
 
}
std::string makeWIFI_WaveformName(const MP5800_WIFI_CONFIG& wifiConfig)
{
	std::string tempWIFI_Standard;
	std::string tempWIFI_ChannelBandwidth;
	std::string tempWIFI_DataRate;
	tempWIFI_Standard = wifiStandard[wifiConfig.Standard] ;
	tempWIFI_ChannelBandwidth = wifiChannelBandwidth[wifiConfig.ChannelBandwidth] ;
	tempWIFI_DataRate = wifiDataRate[wifiConfig.DataRate] ;
	 

	return (tempWIFI_Standard  + tempWIFI_ChannelBandwidth + tempWIFI_DataRate) + ".mp5000vsg";
} 

void makeBT_WaveformMap()
{
	btPackageType.insert( std::pair <BT_PACKET_TYPE, std::string > ( DH1, "BT_DH1") );
	btPackageType.insert( std::pair <BT_PACKET_TYPE, std::string > ( DH3, "BT_DH3") );
	btPackageType.insert( std::pair <BT_PACKET_TYPE, std::string > ( DH5, "BT_DH5") );
	btPackageType.insert( std::pair <BT_PACKET_TYPE, std::string > ( _2DH1, "BT_2DH-1") );
	btPackageType.insert( std::pair <BT_PACKET_TYPE, std::string > ( _2DH3, "BT_2DH-3") );
	btPackageType.insert( std::pair <BT_PACKET_TYPE, std::string > ( _2DH5, "BT_2DH-5") );
	btPackageType.insert( std::pair <BT_PACKET_TYPE, std::string > ( _3DH1, "BT_3DH-1") );
	btPackageType.insert( std::pair <BT_PACKET_TYPE, std::string > ( _3DH3, "BT_3DH-3") );
	btPackageType.insert( std::pair <BT_PACKET_TYPE, std::string > ( _3DH5, "BT_3DH-5") );
	btPackageType.insert( std::pair <BT_PACKET_TYPE, std::string > ( LE, "BT_LE") );

	btDataType.insert( std::pair <BT_DATA_TYPE, std::string > ( All_0, "_All 0") );
	btDataType.insert( std::pair <BT_DATA_TYPE, std::string > ( All_1, "_All 1") );
	btDataType.insert( std::pair <BT_DATA_TYPE, std::string > ( All_10, "_Alt 10") );
	btDataType.insert( std::pair <BT_DATA_TYPE, std::string > ( Random, "_Random") );
	btDataType.insert( std::pair <BT_DATA_TYPE, std::string > ( Four1Four0, "_11110000") );
	btDataType.insert( std::pair <BT_DATA_TYPE, std::string > ( PRBS_9, "_PRBS-9") );
	btDataType.insert( std::pair <BT_DATA_TYPE, std::string > ( PRBS_15, "_PRBS-15") );
	btDataType.insert( std::pair <BT_DATA_TYPE, std::string > ( All_01, "_Alt 01") );
	btDataType.insert( std::pair <BT_DATA_TYPE, std::string > ( Four0Four1, "_00001111") );
 
}
std::string makeBT_WaveformName(const MP5800_BT_CONFIG& btConfig)
{
	std::string tempPackageType;
	std::string tempDataType;
	tempPackageType = btPackageType[btConfig.PacketType] ;
	tempDataType = btDataType[btConfig.DataType] ;

	if (  btConfig.PacketType  == LE)
	{
		switch ( btConfig.DataType ) 
		{
		    case All_10 : tempDataType = "_ALT10"; break;
			case All_01 : tempDataType = "_ALT01"; break;
			case PRBS_9 : tempDataType = "_PRBS9"; break;
		}
	}

	return (tempPackageType + tempDataType ) + ".mp5000vsg";
} 
 

int mp5800Index( const RF_PORT rfPort )
{
	return rfPort/MP5800_PORT_NOs;
}
 
int sampingRateMapping(MP5800_VSA_Sampling_Rate samplingRate)
{  
	switch (samplingRate)
	{
	case _5_MHz: return 5 ;
	case _10_MHz: return 10;
	case _20_MHz: return 20;
	case _40_MHz: return 40;
	case _80_MHz: return 80;
	case _160_MHz : return 160;
	}
}



UINT initSub()
{
  UINT failure;

  bInit = true;
             makeBT_WaveformMap();
			 makeWIFI_WaveformMap();
		    testerConfig.Port_Configuration.RF1 =   VSG1;
            testerConfig.Port_Configuration.RF2  =  VSG1;
            testerConfig.Port_Configuration.RF3  =  VSG1;
            testerConfig.Port_Configuration.RF4  =  VSG1;
      
            testerConfig.Port_Configuration.loadCableLoss = false;
       //     testerConfig.Distribution_Mode = true;
            
            failure = MP5800_setConfig(RF_PORT_1, testerConfig);                
             if ( failure ) return failure;
            
            //2. set vsg --to load waveform
            vsgConfig.Frequency_MHz = 2440 ;
			 vsgConfig.Waveforms_Playback.MOD = CW;
	        vsgConfig.Waveforms_Playback.Count = 0 ;
			 vsgConfig.Waveforms_Playback.Gap_us = 0 ;
			vsgConfig.Output_Power_dBm = -60;
           vsgConfig.Waveforms_Playback.LoadFileName = "BER_gfsk_120M_192ms_no_header.wfm";
			// vsgConfig.Waveforms_Playback.LoadFileName = NULL;
            failure  = MP5800_setVSG( RF_PORT_1, vsgConfig );  // to load waveform
             if ( failure ) return failure;
            
            //3. set vgas
                
            vsaConfig.RBW.RBW =  _500_Hz;            
            vsaConfig.Setting.Trigger_Type = Immediate    ;  
            vsaConfig.Sampling_Rate = _10_MHz;
          
            vsaConfig.Frequency_MHz = 2440 ;
            vsaConfig.Reference_level_dBm = 5; 
            vsaConfig.Setting.Capture_Length_ms = 1;
			vsaConfig.Setting.Offset_us = 0;
            failure  = MP5800_setVSA( RF_PORT_1, vsaConfig );   
			  if ( failure ) return failure;
			 failure = expMV2_SetVsaTriggerOffsetUs( RF_PORT_1, vsaConfig.Setting.Offset_us);   
            if ( failure ) return failure;
           
            // fsk conditon 
            fskConfig.Solution = Freq_Deviation;
            fskConfig.Filter_Type = GAUSSIAN;
            fskConfig.FSK_Type = _2_FSK;
            fskConfig.Symbol_Phase_Coutinuity =  Continuous;
            fskConfig.Filter_Length = 6;
            fskConfig.BT = 0.5;
            fskConfig.Symbol_Rate_MHz = 1.0;
            fskConfig.Freq_Dev_KHz = 250;

     bInit = false;
	 return MP5800_SUCCESS;
	 
}

/* ---------------------------------------------------------------------------
**	MP5800 Connection
** ------------------------------------------------------------------------- */
MP5800_API  UINT   MP5800_connect( const char *IniFilePath , const bool bDisplay , const bool bDebugMode)
{
    
	UINT failure = MP5800_ERROR ;
	failure =  expMV2_InitFunction( IniFilePath );
	if (failure) 
	{
		return MP5800_INITIAL_FAIL;
	}
	
	failure = expMV2_SetDisplayOnOffByConnection( bDisplay );
 
	if (failure) {
		return MP5800_SET_DISPLAY_ON_OFF_FAIL;
	}
	 
	failure = initSub();
    if (failure) {
		return MP5800_SET_INITAL_PARAMETER_FAIL;
	}
	bDebug = bDebugMode;
	
	return MP5800_SUCCESS;
	 
}

// ----------------------------------------------------------------------------

MP5800_API  UINT  MP5800_disconnect( void )
{
	UINT result = MP5800_ERROR;
	result =  expMV2_CloseAllServer();
	if (result) 
	{
		return MP5800_DISCONNECT_ERROR;
	}
 
	
	return MP5800_SUCCESS;
	
}

/* ---------------------------------------------------------------------------
**	MP5800 spectrum on off cotrol
** ------------------------------------------------------------------------- */

MP5800_API  UINT   MP5800_setSpectrumOnOff( const RF_PORT PortIndex, const MP5800_SWITCH OnOff )
{
	UINT failure = MP5800_ERROR;
    failure = expMV2_SetSpectrumOnOff(PortIndex, OnOff?true:false);
	if (failure)
	{
		return MP5800_SET_SPECTRUM_ERROR;
	}
	
	return MP5800_SUCCESS;
	
}
// --------------------------------------------------------------------------
MP5800_API  UINT  MP5800_setCableLossOnOff ( const RF_PORT rfPort, const MP5800_SWITCH OnOff)
{
   UINT failure = MP5800_ERROR;
 
    failure = expMV2_SetCableLossEnable(rfPort, OnOff ? true : false);
	if (failure)
	{
		return MP5800_SET_CABLE_LOSS_ON_OFF_ERROR;
	}
	 
		return MP5800_SUCCESS;
	 
}
/* ---------------------------------------------------------------------------
**	MP5800 PortConfigon off cotrol
** ------------------------------------------------------------------------- */

PORT_MODE  assingPortConfiguration( MP5800_SWITCH portConfig  )
{
	switch (portConfig)
	{
	   case VSG1 :  return  PORT_MODE_TX ;  
	   case VSA1 :  return  PORT_MODE_RX ; 
	   case SWITCH_OFF : return    PORT_MODE_OFF ; 
    }
}
//---------------------------------------------------------------------------------------------
MP5800_API  UINT   MP5800_setPortConfiguration( const RF_PORT rfPort , const MP5800_CONFIG& config ) // tester API
{
	
	UINT failure = MP5800_ERROR ;
 	int index = mp5800Index(rfPort);

	if (   ( bDebug) || (bInit) ||
	   (  mp5800TesterConfig[index].Port_Configuration.RF1 != config.Port_Configuration.RF1 
			||  mp5800TesterConfig[index].Port_Configuration.RF2 != config.Port_Configuration.RF2  
			||  mp5800TesterConfig[index].Port_Configuration.RF3 != config.Port_Configuration.RF3 
			||  mp5800TesterConfig[index].Port_Configuration.RF4 != config.Port_Configuration.RF4 ) ){
	 
		failure =  expMV2_Set_4Port_Switch( rfPort,
				  assingPortConfiguration( config.Port_Configuration.RF1),
				  assingPortConfiguration( config.Port_Configuration.RF2),
				  assingPortConfiguration( config.Port_Configuration.RF3),
				  assingPortConfiguration( config.Port_Configuration.RF4) ); 
		if (failure) {
			return MP5800_Set_4Port_Switch_FAIL_ERROR;
		}
					mp5800TesterConfig[index].Port_Configuration.RF1 = config.Port_Configuration.RF1 ;
					mp5800TesterConfig[index].Port_Configuration.RF2 = config.Port_Configuration.RF2 ; 
					mp5800TesterConfig[index].Port_Configuration.RF3 = config.Port_Configuration.RF3 ;
					mp5800TesterConfig[index].Port_Configuration.RF4 = config.Port_Configuration.RF4 ;
	
	}
		return MP5800_SUCCESS;
	 
}

// -----------------------------------------------------------------------------
MP5800_API UINT  MP5800_setConfig( const RF_PORT rfPort ,	const MP5800_CONFIG&  config) // tester API
{
	UINT failure = MP5800_ERROR ;
	int index = mp5800Index(rfPort);
	/*
	if ( config.External_Oscillator )	{
		// to do set external oscillator --------------- unfinished
	}

	if ( config.External_Lo ){
		// to do set external lo ----------------------- unfinished
	}
	*/
 /*
	if ( ( bDebug ) || ( bInit) || ( mp5800TesterConfig[index].Distribution_Mode != config.Distribution_Mode ) ) {
			failure = expMV2_SetDistributionModeOnOff( rfPort, config.Distribution_Mode );
			if ( failure ) {
				return MP5800_SET_DISTRIBUTION_MODE_ERROR ;
			}	
			mp5800TesterConfig[index].Distribution_Mode = config.Distribution_Mode ;
	} 
*/  
	failure = expMV2_SetDistributionModeOnOff( rfPort, true );// always distrubutedMode
	if ( ( bDebug ) || ( bInit) || ( mp5800TesterConfig[index].Port_Configuration.loadCableLoss != config.Port_Configuration.loadCableLoss ) ) {
		failure = expMV2_SetCableLossEnable ( rfPort , config.Port_Configuration.loadCableLoss);
		if ( failure ) {
			return MP5800_SET_ENABLE_CABLE_LOSS_ERROR ;
		}
		mp5800TesterConfig[index].Port_Configuration.loadCableLoss = config.Port_Configuration.loadCableLoss ;
	}
	 

	// --------------------------- loop back ----------------------

	


	
	if (   ( bDebug ) || ( bInit) ||
	   (  mp5800TesterConfig[index].Port_Configuration.RF1 != config.Port_Configuration.RF1 
			||  mp5800TesterConfig[index].Port_Configuration.RF2 != config.Port_Configuration.RF2  
			||  mp5800TesterConfig[index].Port_Configuration.RF3 != config.Port_Configuration.RF3 
			||  mp5800TesterConfig[index].Port_Configuration.RF4 != config.Port_Configuration.RF4 ) ){

     if (    config.Port_Configuration.RF1 == VSA1 
		||  config.Port_Configuration.RF2 == VSA1
		||  config.Port_Configuration.RF3 == VSA1
		||  config.Port_Configuration.RF4 == VSA1 ) {
		mp5800LoopBack[index] = true;
	 }
	 else
	 { 
		 mp5800LoopBack[index] = false;
	 }
	 
		failure =  expMV2_Set_4Port_Switch( rfPort,
				  assingPortConfiguration( config.Port_Configuration.RF1),
				  assingPortConfiguration( config.Port_Configuration.RF2),
				  assingPortConfiguration( config.Port_Configuration.RF3),
				  assingPortConfiguration( config.Port_Configuration.RF4) ); 
		if (failure) {
			return MP5800_Set_4Port_Switch_FAIL_ERROR;
		}
		mp5800TesterConfig[index].Port_Configuration.RF1 = config.Port_Configuration.RF1 ;
		mp5800TesterConfig[index].Port_Configuration.RF2 = config.Port_Configuration.RF2 ; 
		mp5800TesterConfig[index].Port_Configuration.RF3 = config.Port_Configuration.RF3 ;
		mp5800TesterConfig[index].Port_Configuration.RF4 = config.Port_Configuration.RF4 ;
	
	}

	 return MP5800_SUCCESS;
}

// -------------------------------------------------------------------------------------
MP5800_API UINT  MP5800_setMode( const RF_PORT rfPort ,const MP5800_STANDARD  standard) // tester API
{
	UINT failure = MP5800_ERROR ;
	int index = mp5800Index(rfPort);
	if ( ( bDebug) || ( bInit) || ( mp5800TesterConfigStandard[index] !=   standard) ) {
		failure = expMV2_SetMeasureMode( rfPort, standard );
		if ( failure ) 	{
			return  MP5800_SET_STANDARD_ERROR;
		}
		 mp5800TesterConfigStandard[index] =   standard;
	}
	return MP5800_SUCCESS;
}

// ----------------------------------------------------------------------------------
MP5800_API UINT  MP5800_loadWaveform( const RF_PORT rfPort, const char *waveFormFileName)  // tester API
{
	expMV2_LoadMemory(rfPort,waveFormFileName );
	return MP5800_SUCCESS;
}

/* ---------------------------------------------------------------------------
**	MP5800 VSG/VSA function, need set counter
** ------------------------------------------------------------------------- */
 

MP5800_API UINT  MP5800_setVSG( const RF_PORT rfPort ,  const MP5800_VSG_CONFIG& vsgConfig)	 // set tester API
{
	
	UINT failure = MP5800_ERROR ;
	int index = mp5800Index(rfPort);
 
	if ( ( bDebug) || ( bInit) || ( mp5800TesterConfigStandard[index] !=   GPRF) ) {
		failure = expMV2_SetMeasureMode( rfPort, GPRF );
		if ( failure ) 	{
			return  MP5800_SET_STANDARD_ERROR;
		}
		mp5800TesterConfigStandard[index] =  GPRF;
	}
	
	// close power at first
	if ( (bDebug) || ( bInit) || ( mp5800VsgConfig[index].Waveforms_Playback.RF != SWITCH_OFF ) ) {
		failure  = expMV2_Rf_OnOff(rfPort, SWITCH_OFF);  // close power
		if ( failure  ) {
			return MP5800_SET_RF_ON_OFF_FAIL_ERROR;
		}
		mp5800VsgConfig[index].Waveforms_Playback.RF = SWITCH_OFF;
	}
// ================================================================================
//	Freq_Band  Freq_Band ;
//	Channel  Channel ;
//	UINT Frequency_MHz;	 
//	double Output_Power_dBm ; // -90 ~ + 30
	if ( (bDebug) || ( bInit)
		|| ( mp5800VsgConfig[index].Frequency_MHz !=  vsgConfig.Frequency_MHz )
		|| ( mp5800VsaConfig[index].Frequency_MHz !=  vsgConfig.Frequency_MHz ) ){
			failure = expMV2_SetCenterFrequency(rfPort , (double) vsgConfig.Frequency_MHz )  ;
			if ( failure )
			{ 
				return MP5800_SET_CENTER_FREQUENCY_ERROR;
			}
			mp5800VsgConfig[index].Frequency_MHz =  vsgConfig.Frequency_MHz  ;
			mp5800VsaConfig[index].Frequency_MHz =  vsgConfig.Frequency_MHz  ;
	}
	// set output pwr
	 
	if ( (bDebug) ||( bInit) || ( mp5800VsgConfig[index].Output_Power_dBm !=  vsgConfig.Output_Power_dBm ) ){
			failure = MP5800_ERROR;
			failure =  expMV2_SetTxOutputLevel( rfPort, (double) vsgConfig.Output_Power_dBm );  
			if ( failure )
			{
				return MP5800_SET_TX_POWER_ERROR;
			}
			mp5800VsgConfig[index].Output_Power_dBm =  vsgConfig.Output_Power_dBm;
	}
//--------------------------------------------------------------------------------------
	// set CW 
		//char* LoadFileName;
 	if ( vsgConfig.Waveforms_Playback.LoadFileName != NULL)
	{
	if ( (bDebug) || ( bInit) || ( mp5800VsgConfig[index].Waveforms_Playback.LoadFileName !=  vsgConfig.Waveforms_Playback.LoadFileName ) ){
	  failure = MP5800_ERROR;
	   failure =  expMV2_LoadMemory(rfPort, vsgConfig.Waveforms_Playback.LoadFileName );
	 
	  if ( failure )
	  {
			return MP5800_LOAD_WAVEFORM_ERROR;
	  }
	  mp5800VsgConfig[index].Waveforms_Playback.LoadFileName =  vsgConfig.Waveforms_Playback.LoadFileName ;
	}
	}
	//	MP5800_VSG_RF   RF;  ------------------------ pull to start run
	//	MP5800_VSG_MOD  MOD;
	//	UINT  Count;   ------------------------------ to do
	//	UINT Gap_us;  ------------------------------- to do
	//	bool RUN;   -------------------------------- pull to start run

	// if ( (bDebug) || ( bInit) || ( mp5800VsgConfig[index].Waveforms_Playback.MOD !=  vsgConfig.Waveforms_Playback.MOD ) )
	// becaues the RF_ON_off will couple
	{
		if  ( vsgConfig.Waveforms_Playback.MOD == CW)
		{
			failure = MP5800_ERROR;
	 
			failure =  expMV2_ModulationOnOff( rfPort, false)  ;
			if ( failure )
			{
				 return MP5800_SET_CW_FAIL_ERROR;
			}
		}

		// set modulation
		else if  ( vsgConfig.Waveforms_Playback.MOD == WAVE ) 
		{
			failure = MP5800_ERROR;
	 
			failure =  expMV2_ModulationOnOff( rfPort, true)  ;
			if ( failure )
			{
				return MP5800_SET_MODULATION_FAIL_ERROR;
			}
		}
		mp5800VsgConfig[index].Waveforms_Playback.MOD =  vsgConfig.Waveforms_Playback.MOD;
	}

	// coout
	if ( (bDebug) || ( bInit) || ( mp5800VsgConfig[index].Waveforms_Playback.Count !=  vsgConfig.Waveforms_Playback.Count ) ){
         failure = MP5800_ERROR;
	     failure =  expMV2_SetVsgPlayCount(rfPort, vsgConfig.Waveforms_Playback.Count );
		  if ( failure )
		  {
				return MP5800_LOAD_WAVEFORM_ERROR;
		  }
	      mp5800VsgConfig[index].Waveforms_Playback.Count =  vsgConfig.Waveforms_Playback.Count ;
	}

		// coout
	if ( (bDebug) || ( bInit) || ( mp5800VsgConfig[index].Waveforms_Playback.Gap_us!=  vsgConfig.Waveforms_Playback.Gap_us) ){
         failure = MP5800_ERROR;
	     failure =  expMV2_SetVsgPlayGapUs(rfPort, vsgConfig.Waveforms_Playback.Gap_us );
		  if ( failure )
		  {
				return MP5800_SET_PALYBACK_GAP_TIME_ERROR;
		  }
	      mp5800VsgConfig[index].Waveforms_Playback.Gap_us=  vsgConfig.Waveforms_Playback.Gap_us ;
	}
// ================================================================================    
	  
//  struct _Impairments
//	{
//		double DC_Offset;  ---------------------- to do 
//		double Ampl_Offset; ---------------------- to do 
//		double Phase_Offset; --------------------- to do
	 
//		int  CW_Offset_MHz;
//	}   Impairments;


	if ( (bDebug) || ( bInit) || ( mp5800VsgConfig[index].Impairments.CW_Offset_MHz !=  vsgConfig.Impairments.CW_Offset_MHz ) ){
       failure = MP5800_ERROR;
	   failure = expMV2_SetCWFrequencyOffset ( rfPort, vsgConfig.Impairments.CW_Offset_MHz );
		 
	     if ( failure )
		{
			return MP5800_SET_CW_FREQUENCY_OFFSET_ERROR;
		}
		 mp5800VsgConfig[index].Impairments.CW_Offset_MHz =  vsgConfig.Impairments.CW_Offset_MHz ;
	}


	return MP5800_SUCCESS;
	 

}
											   
// ---------------------------------------------------------------------------

MP5800_API UINT  MP5800_setVSG_Run( const RF_PORT rfPort, bool vsgOnOff )  // teser API, use port to control the
{
	UINT failure = MP5800_ERROR ;
	int index = mp5800Index(rfPort);

 
	if ( ( bDebug) || ( bInit) || ( mp5800TesterConfigStandard[index] !=   GPRF) ) {
		failure = expMV2_SetMeasureMode( rfPort, GPRF );
		if ( failure ) 	{
			return  MP5800_SET_STANDARD_ERROR;
		}
		mp5800TesterConfigStandard[index] =  GPRF;
	}

 
	{
			failure = expMV2_Rf_OnOff(rfPort,vsgOnOff);
			if ( failure )
			{
				  return MP5800_SET_RF_ON_OFF_FAIL_ERROR;
			}
	        mp5800VsgConfig[index].Waveforms_Playback.RF = vsgOnOff?SWITCH_ON:SWITCH_OFF ;
	}

	if ( vsgOnOff)
	{
		failure =  expMV2_Set_4Port_Switch( rfPort,
				  assingPortConfiguration( mp5800TesterConfig[index].Port_Configuration.RF1),
				  assingPortConfiguration( mp5800TesterConfig[index].Port_Configuration.RF2),
				  assingPortConfiguration( mp5800TesterConfig[index].Port_Configuration.RF3),
				  assingPortConfiguration( mp5800TesterConfig[index].Port_Configuration.RF4) ); 
		if (failure) {
			return MP5800_Set_4Port_Switch_FAIL_ERROR;
		}
	}
	else
	{
		failure =  expMV2_Set_4Port_Switch( rfPort,   PORT_MODE_OFF,  PORT_MODE_OFF , PORT_MODE_OFF,  PORT_MODE_OFF );
				    
		if (failure) {
			return MP5800_Set_4Port_Switch_FAIL_ERROR;
		}

		 return MP5800_SUCCESS;
	}   
}

// ---------------------------------------------------------------------------
    


MP5800_API UINT  MP5800_setVSA( const RF_PORT rfPort,  const MP5800_VSA_CONFIG& vsaConfig)    // tester API
{

    UINT failure = MP5800_ERROR ;
	int index = mp5800Index(rfPort);
 
	if ( ( bDebug) || ( bInit) || ( mp5800TesterConfigStandard[index] !=   GPRF) ) {
		failure = expMV2_SetMeasureMode( rfPort, GPRF );
		if ( failure ) 	{
			return  MP5800_SET_STANDARD_ERROR;
		}
		mp5800TesterConfigStandard[index] =  GPRF;
		
	}
// -----------------------------------------------------------------------------
//	Freq_Band  Freq_Band ;
//	Channel  Channel ;
//	UINT Frequency_MHz;
//	MP5800_VSA_Sampling_Rate    Sampling_Rate;
//	double Reference_level_dBm ; // -90 ~ + 30

    //if ( ( bDebug) || ( bInit) || ( mp5800VsaConfig[index].Demodulation !=   _NULL ) ) 
	{
	    failure = expMV2_Set_GPRF_DemodulationType( rfPort, _NULL );
		if ( failure ) {
			return MP5800_SET_DEMODULATION_ERROR;
		}
		 mp5800VsaConfig[index].Demodulation =   _NULL;
	}
	     

	if ( ( bDebug) || ( bInit) 
		|| ( mp5800VsaConfig[index].Frequency_MHz !=  vsaConfig.Frequency_MHz )
		|| ( mp5800VsgConfig[index].Frequency_MHz !=  vsaConfig.Frequency_MHz )  ) {
	 	 failure = expMV2_SetCenterFrequency(rfPort, vsaConfig.Frequency_MHz);  
		  if (failure) {
			 return MP5800_SET_CENTER_FREQUENCY_ERROR;
		 }
		  mp5800VsaConfig[index].Frequency_MHz =  vsaConfig.Frequency_MHz;
		  mp5800VsgConfig[index].Frequency_MHz =  vsaConfig.Frequency_MHz;
	}
        
	// set sampling rate
#ifndef MP5000
	if ( ( bDebug) || ( bInit) || ( mp5800VsaConfig[index].Sampling_Rate !=  vsaConfig.Sampling_Rate ) ) {
	      failure = expMV2_SetFreqSpan( rfPort, sampingRateMapping(vsaConfig.Sampling_Rate));
		 if (failure) {	 
			 return MP5800_SET_SAMPLING_RATE_FAIL_ERROR;
		 }
		 mp5800VsaConfig[index].Sampling_Rate =  vsaConfig.Sampling_Rate;
	}
#endif
	  if ( ( bDebug) || ( bInit) || ( mp5800VsaConfig[index].Reference_level_dBm !=  vsaConfig.Reference_level_dBm ) ) {
	     failure = expMV2_SetRxReferenceLevel( rfPort, vsaConfig.Reference_level_dBm );
		  if (failure) {
			 return MP5800_SET_REFERENCE_LEVEL_FAIL_ERROR;
		 }
		  mp5800VsaConfig[index].Reference_level_dBm =  vsaConfig.Reference_level_dBm ;
	  } 


//	 bool AGC; ------------------------------- to do
//	 bool Run;	------------------------------- to do 
//----------------------------------------------------------------------------------------
	
     // expMV2_SetRxRBW(RF_PORT_1,RBW_100K_Hz);//Aeroflex RBW=244K
/*	struct _RBW
   {
		MP5800_VSA_RBW  RBW ;
		MP5800_VSA_Window_Type  Window_Type;   ----------------------- to do
    } RBW;
	*/
	if ( ( bDebug) || ( bInit) || ( mp5800VsaConfig[index].RBW.RBW !=  vsaConfig.RBW.RBW) ) {
       failure = expMV2_SetRxRBW(rfPort,(RBW) vsaConfig.RBW.RBW);  
		 if ( failure)
		 {
			 return MP5800_SET_RBW_ERROR;
		 }
		 mp5800VsaConfig[index].RBW.RBW =  vsaConfig.RBW.RBW;
	}
//-----------------------------------------------------------------------------------------      
/*	struct _Setting  ----- need check
 	{
		double Capture_Length_ms;
		UINT count;
		UINT Packet;
		bool VSA_CRC_Check;
		MP5800_VSA_Trigger_Type   Trigger_Type;
		double Threshold_dB;
		UINT Gap_Time_us;
		int Offset_us;
		UINT Timeout_ms;
	}  Setting;
*/
	if ( ( bDebug) || ( bInit) || ( mp5800VsaConfig[index].Setting.Capture_Length_ms !=  vsaConfig.Setting.Capture_Length_ms) ) {
		  failure = expMV2_SetVsaCaptureLength(rfPort, vsaConfig.Setting.Capture_Length_ms*0.001);  
		  if ( failure )
		 {
			 return MP5800_SET_CAPTURE_LENGTH_ERROR;
		 }
         mp5800VsaConfig[index].Setting.Capture_Length_ms =  vsaConfig.Setting.Capture_Length_ms ;
	}

	if ( ( bDebug) || ( bInit) || ( mp5800VsaConfig[index].Setting.Trigger_Type !=  vsaConfig.Setting.Trigger_Type) ) {
          failure = expMV2_SetVsaTriggerType(rfPort, vsaConfig.Setting.Trigger_Type);  
		  if (failure)
		 {
			 return MP5800_SET_TRIGGER_TYPE_ERROR;
		 }
		  mp5800VsaConfig[index].Setting.Trigger_Type =  vsaConfig.Setting.Trigger_Type;
	}


	
	  if (vsaConfig.Setting.Trigger_Type != Immediate)
	  {
		  if ( ( bDebug) || ( bInit) || ( mp5800VsaConfig[index].Setting.Gap_Time_us !=  vsaConfig.Setting.Gap_Time_us ) ) {
		      failure = expMV2_SetVsaGapTime(rfPort, vsaConfig.Setting.Gap_Time_us*0.000001);  
			  if ( failure )  {
				 return MP5800_SET_GAP_TIME_ERROR;
			 }
			  mp5800VsaConfig[index].Setting.Gap_Time_us =  vsaConfig.Setting.Gap_Time_us ;
		  }

		  if ( ( bDebug) || ( bInit) || ( mp5800VsaConfig[index].Setting.Offset_us !=  vsaConfig.Setting.Offset_us ) ) {
		      failure = expMV2_SetVsaTriggerOffsetUs(rfPort, vsaConfig.Setting.Offset_us);  
			  if ( failure )  {
				 return MP5800_SET_TRIGGER_OFFSET_ERROR;
			 }
			  mp5800VsaConfig[index].Setting.Offset_us =  vsaConfig.Setting.Offset_us ;
		  }

	  }
	  // -------------------- need to verify
	  /* 
	  if ( ( bDebug) || ( mp5800VsaConfig[index].Setting.Timeout_ms !=  vsaConfig.Setting.Timeout_ms ) )
	  {
	    failure = expMV2_SetVsaTriggerTimeOut(rfPort, vsaConfig.Setting.Timeout_ms*0.001);  
		 if (failure) 	 {
			 return MP5800_SET_TRIGGER_TIME_OUT_ERROR;
		 }
		 mp5800VsaConfig[index].Setting.Timeout_ms =  vsaConfig.Setting.Timeout_ms;
	  }
	  */
    /*
      expMV2_SetDigitalDownConvertor(RF_PORT_1,true,(2439.8-2441.)*1.0E6);
       expMV2_SetMark(RF_PORT_1,2439.8);  // ddc
      // expMV2_SetMark(RF_PORT_1,2439.8);  // no ddc
	  */
	return MP5800_SUCCESS;
}
  
// ---------------------------------------------------------------------------

MP5800_API UINT  MP5800_getVSA_Value(const RF_PORT rfPort, MP5800_VSA_MEASURE& vsMeasure )  // port command
{
	UINT failure = MP5800_ERROR ;
	int index = mp5800Index(rfPort);
 
	if ( ( bDebug) || ( bInit) || ( mp5800TesterConfigStandard[index] !=   GPRF) ) {
		failure = expMV2_SetMeasureMode( rfPort, GPRF );
		if ( failure ) 	{
			return  MP5800_SET_STANDARD_ERROR;
		}
		mp5800TesterConfigStandard[index] =  GPRF;
	}

	 
	 //if ( ( bDebug) || ( bInit) || ( mp5800VsaConfig[index].Demodulation !=   _NULL ) )
	 {
	    failure = expMV2_Set_GPRF_DemodulationType( rfPort, _NULL );
		if ( failure ) {
			return MP5800_SET_DEMODULATION_ERROR;
		}
		 mp5800VsaConfig[index].Demodulation =   _NULL;
	}
	     
    // need to modify
	 if ( !mp5800LoopBack[index] )  {
	    failure =expMV2_SetVsaRx_Port(rfPort);
		if( failure) {
			return MP5800_SET_VSA_PORT_ERROR;
		}
	 }

	failure = MP5800_ERROR;
	failure = expMV2_GetPowerByTime(rfPort, vsMeasure.Channel_Power_dBm);   
	 
	if( failure)
	 {
		 vsMeasure.Channel_Power_dBm = MP5800_ERROR_VALUE;
		 vsMeasure.Peak_Frequency_MHz = MP5800_ERROR_VALUE,
		 vsMeasure.Peak_Power_dBm =  MP5800_ERROR_VALUE;
		 return MP5800_ERROR;
	 }

	failure = expMV2_GetPeakData(RF_PORT_1,vsMeasure.Peak_Frequency_MHz,vsMeasure.Peak_Power_dBm);   
	if ( failure )
	{   
		 vsMeasure.Channel_Power_dBm = MP5800_ERROR_VALUE;
		 vsMeasure.Peak_Frequency_MHz = MP5800_ERROR_VALUE,
		 vsMeasure.Peak_Power_dBm =  MP5800_ERROR_VALUE;
		 return MP5800_ERROR;
	}
 
	 return MP5800_SUCCESS;
	 
}



// ----------------------------------------------------------------------------
// FSK demodulation
// ----------------------------------------------------------------------------
MP5800_API UINT  MP5800_setFSK_VSA( const RF_PORT rfPort, const MP5800_FSK_CONFIG& fskConfig, const MP5800_VSA_CONFIG& vsaConfig)
{
	UINT failure = MP5800_ERROR ;
	int index = mp5800Index(rfPort);
 
	if ( ( bDebug) || ( bInit) || ( mp5800TesterConfigStandard[index] !=   GPRF) ) {
		failure = expMV2_SetMeasureMode( rfPort, GPRF );
		if ( failure ) 	{
			return  MP5800_SET_STANDARD_ERROR;
		}
		mp5800TesterConfigStandard[index] =  GPRF;
	}

	 failure = MP5800_setVSA(rfPort, vsaConfig);
	 if (failure)
	 {
		 return failure;
	 }
/*
	 // need to modify
     failure = expMV2_SetVsaRx_Port(rfPort);
	*/
	 if (     ( bDebug) || ( bInit)  
		 	  || ( mp5800FskConfig[index].Solution != fskConfig.Solution )
			  ||  ( mp5800FskConfig[index].Filter_Type != fskConfig.Filter_Type )
			  ||  ( mp5800FskConfig[index].Symbol_Phase_Coutinuity != fskConfig.Symbol_Phase_Coutinuity ) 
			  ||  ( mp5800FskConfig[index].FSK_Type != fskConfig.FSK_Type ) 
			  ||  ( mp5800FskConfig[index].Filter_Length != fskConfig.Filter_Length ) 
			  ||  ( mp5800FskConfig[index].BT != fskConfig.BT ) 
			  ||  ( mp5800FskConfig[index].Symbol_Rate_MHz != fskConfig.Symbol_Rate_MHz ) 
			  ||  ( mp5800FskConfig[index].Freq_Dev_KHz != fskConfig.Freq_Dev_KHz ) 	  ){

			failure = expMV2_SetFskDemodulation( rfPort ,
						 fskConfig.Solution, 
						 fskConfig.Filter_Type, 
						 fskConfig.Symbol_Phase_Coutinuity,
						 fskConfig.FSK_Type, 
						 fskConfig.Filter_Length,
						 fskConfig.BT,
						 fskConfig.Symbol_Rate_MHz,
						 fskConfig.Freq_Dev_KHz );

			 if ( failure )
			 {
				 return MP5800_SET_FSK_VSA_ERROR;
			 }
	
	        mp5800FskConfig[index].Solution = fskConfig.Solution ;
			mp5800FskConfig[index].Filter_Type = fskConfig.Filter_Type;
			mp5800FskConfig[index].Symbol_Phase_Coutinuity = fskConfig.Symbol_Phase_Coutinuity ;
			mp5800FskConfig[index].FSK_Type = fskConfig.FSK_Type ;
			mp5800FskConfig[index].Filter_Length = fskConfig.Filter_Length; 
			mp5800FskConfig[index].BT = fskConfig.BT ;
			mp5800FskConfig[index].Symbol_Rate_MHz = fskConfig.Symbol_Rate_MHz ;
			mp5800FskConfig[index].Freq_Dev_KHz = fskConfig.Freq_Dev_KHz ;	  ;
		     
	 }
	  return MP5800_SUCCESS;
}


// ----------------------------------------------------------------------------
MP5800_API UINT  MP5800_getFSK_VSA_Value( const RF_PORT rfPort, MP5800_FSK_MEASURE& fskMeasure )
{
	UINT failure = MP5800_ERROR;
	int index = mp5800Index(rfPort);
	double fskMeasure_Freq_Dev_MHz = 0;
	double fskMeasure_Freq_Drift_MHz =0.;
	double fskMeasure_Freq_Offset_MHz = 0.;
	double fskMeasure_Dev_ERROR_MHz = 0.;
	double fskMeasure_FSK_ERROR_MHz = 0.;

	if ( ( bDebug) || ( bInit) || ( mp5800TesterConfigStandard[index] !=   GPRF) ) {
		failure = expMV2_SetMeasureMode( rfPort,GPRF );
		if ( failure ) {
			return MP5800_SET_STANDARD_ERROR;
		}
		mp5800TesterConfigStandard[index] =   GPRF ;
	}


	if ( ( bDebug) || ( bInit) || ( mp5800VsaConfig[index].Demodulation !=   FSK) ) {
			failure = expMV2_Set_GPRF_DemodulationType( rfPort, FSK );
			if ( failure ) {
				return MP5800_SET_FSK_DEMODULATION_ERROR;
			}
        mp5800VsaConfig[index].Demodulation =   FSK;
	}
	 
	 if ( !mp5800LoopBack[index] )  {
	    failure =expMV2_SetVsaRx_Port(rfPort);
		if( failure) {
			return MP5800_SET_VSA_PORT_ERROR;
		}
	 }


    // direct read data, so do not set again

	if ( mp5800FskConfig[index].Solution  == Freq_Deviation )
	{
		
		failure = expMV2_GetFSK_FreqDeviation( rfPort , fskMeasure_Freq_Dev_MHz );
		fskMeasure.Freq_Dev_kHz = fskMeasure_Freq_Dev_MHz * 1000.;
		if( failure) {
			return MP5800_GET_FSK_FREQDEVIATION_ERROR;
		}
	}
	else if ( mp5800FskConfig[index].Solution  == All_measurements )
	{
		failure = expMV2_GetFSKDemodulation( rfPort ,fskMeasure_Freq_Dev_MHz, fskMeasure_Freq_Offset_MHz,
			                       fskMeasure_Freq_Drift_MHz, fskMeasure_Dev_ERROR_MHz, fskMeasure_FSK_ERROR_MHz );

		fskMeasure.Freq_Dev_kHz = fskMeasure_Freq_Dev_MHz*1000.;
		fskMeasure.Freq_Offset_kHz = fskMeasure_Freq_Offset_MHz*1000.;
		fskMeasure.Freq_Drift_kHz = fskMeasure_Freq_Drift_MHz*1000.;
		fskMeasure.Dev_ERROR_kHz = fskMeasure_Dev_ERROR_MHz*1000.;
		fskMeasure.FSK_ERROR_kHz = fskMeasure_FSK_ERROR_MHz*1000.;

		if( failure) {
			return MP5800_GET_ALL_MEASUREMENT_ERROR;
		}
	}
	 
	 
	 
		// result = expMV2_SetFskDemodulation( rfPort ,  false ); // close it
		         
		  return MP5800_SUCCESS;
	 
}

// ---------------------------------------------------------------------------
//  wifi  RX
// ---------------------------------------------------------------------------
MP5800_API UINT  MP5800_setWIFI_VSA( const RF_PORT rfPort, const MP5800_WIFI_CONFIG& wifiConfig  , const MP5800_VSA_CONFIG& vsaConfig)
{
	UINT failure = MP5800_ERROR;
	int index = mp5800Index(rfPort);

	if (wifiConfig.Standard == _802_11b) 
	{
	     if ( ( bDebug) || ( bInit) || (mp5800TesterConfigStandard[index] != WIFI_DSSS) )
		{		
				failure = expMV2_SetMeasureMode(rfPort,WIFI_DSSS);
				if ( failure ) 		{
						return MP5800_SET_WIFI_DSSS_ERROR;
				}
			  mp5800TesterConfigStandard[index] = WIFI_DSSS;
		}
	}
	else
	{
	    if ( ( bDebug) || ( bInit) || (mp5800TesterConfigStandard[index] != WIFI_OFDM) )
		{
					
				failure = expMV2_SetMeasureMode(rfPort,WIFI_OFDM);
				if ( failure ) 		{
						return MP5800_SET_WIFI_OFDM_ERROR;
				}
			  mp5800TesterConfigStandard[index] = WIFI_OFDM;
		}
	}
	mp5800WifiConfig[index].Standard =   wifiConfig.Standard;
/*

	if ( ( bDebug) || ( bInit) || ( mp5800WifiConfig[index].Standard !=   wifiConfig.Standard ) ) { 
		if (wifiConfig.Standard != _802_11b) 	{
			failure = expMV2_SetMeasureMode(rfPort,WIFI_OFDM);
			if ( failure ) 		{
					return MP5800_SET_WIFI_OFDM_ERROR;
			}
			mp5800TesterConfigStandard[index] =  WIFI_OFDM;
		}
		else 	{
			failure = expMV2_SetMeasureMode(rfPort,WIFI_DSSS);
			if ( failure ) 		{
					return MP5800_SET_WIFI_DSSS_ERROR;
			}
			mp5800TesterConfigStandard[index] =  WIFI_DSSS;
		}
		mp5800WifiConfig[index].Standard =   wifiConfig.Standard;
	}

 */		 



	failure = expMV2_Set_WIFI_RX( rfPort,                                
	                             vsaConfig.Frequency_MHz,                
								 vsaConfig.Reference_level_dBm,			 			
								 vsaConfig.Setting.Capture_Length_ms*0.001,    						                
								 wifiConfig.Standard,				     
								 wifiConfig.ChannelBandwidth,	            
								 wifiConfig.DataRate,                  
								 false );      

	 if ( failure )
	 {
		 return MP5800_SET_WIFI_RX_ERROR; 
	 }
		/*			                								 
	 result = MP5800_ERROR;
	 double temp1 =  vsaConfig.Setting.Gap_Time_us;
	 temp1 =  temp1 * 0.000001;
	  double temp2 =  vsaConfig.Setting.Offset;
	 temp2 =  temp2 * 0.000001;
	 */
     
	 failure = expMV2_Set_WIFI_BT_RX_Advance( rfPort, 
	                                        (WIFI_BT_TRIGGER_TYPE)vsaConfig.Setting.Trigger_Type,             
	                                      vsaConfig.Setting.Gap_Time_us*0.000001 ,
									    //     temp1,
											 vsaConfig.Setting.Threshold_dB,
									 	 	 vsaConfig.Setting.Offset_us*0.000001,
										      
									     //    temp2,
	                                         wifiConfig.Correction.Frequency_Correction ,   
											 wifiConfig.Correction.Channel_Estimation );
  
      
	  if ( failure)
	 {
		 return MP5800_SET_WIFI_RX_ADVANCE_ERROR; 
	 }
	 
	     return MP5800_SUCCESS;	
	   
}

// ---------------------------------------------------------------------------
MP5800_API UINT  MP5800_getWIFI_VSA_Value(const RF_PORT  rfPort ,MP5800_WIFI_MEASURE& wifiMeasure)
{
	UINT failure = MP5800_ERROR;
	int index = mp5800Index(rfPort);

	 if ( ( bDebug) || ( bInit) || 
	        ( ( mp5800WifiConfig[index].Standard == _802_11b )   &&  ( mp5800TesterConfigStandard[index] != WIFI_DSSS ) ) )
			{  
				 failure = expMV2_SetMeasureMode(rfPort,WIFI_DSSS);
					if ( failure ) 		{
							return MP5800_SET_WIFI_DSSS_ERROR;
					}
				  mp5800TesterConfigStandard[index] = WIFI_DSSS ;
			}
		  

		    if ( ( bDebug) || ( bInit) || 
	        ( ( mp5800WifiConfig[index].Standard != _802_11b )   &&  ( mp5800TesterConfigStandard[index] != WIFI_OFDM ) ) )
			{					
				failure = expMV2_SetMeasureMode(rfPort,WIFI_OFDM);
				if ( failure ) 		{
							return MP5800_SET_WIFI_OFDM_ERROR;
				}
					mp5800TesterConfigStandard[index] = WIFI_OFDM ;
			}
		  
		 
		 
	 
 
	/* 
    failure = expMV2_Get_WIFI_RX( rfPort,	                                                  
						wifiMeasure.TXPowerdBm,                
						wifiMeasure.EVM,                       
						wifiMeasure.bMASK,                      
						wifiMeasure.PhaseError,               
						wifiMeasure.FrequencyError,           
						wifiMeasure.SymbolClockError,        
						wifiMeasure.LoLeakage  
						wifiMeasure.MaskTotalCount,          
						wifiMeasure.MaskErrorCount,           
						wifiMeasure.isBFON,                   
						wifiMeasure.PreamblePower            );
   */
  
	failure = expMV2_Get_WIFI_RX( rfPort,	                                                  
						  wifiMeasure.Power_dBm,        
						  wifiMeasure.EVM_dB,         
						  wifiMeasure.Spectrum_Mask_bool,  
						  wifiMeasure.Phase_Error_deg,
						  wifiMeasure.Freq_Error_kHz,  
						  wifiMeasure.SymClk_Error_ppm,
						  wifiMeasure.Lo_Leakage_dB);
	 

											 
	  if ( failure )
	 { 
		 return MP5800_GET_WIFI_RX_ERROR; 
	 }
	   return MP5800_SUCCESS;	
	  

}


// ---------------------------------------------------------------------------
//  wifi  TX
// ---------------------------------------------------------------------------
MP5800_API UINT  MP5800_setWIFI_VSG( const RF_PORT rfPort, const MP5800_WIFI_CONFIG& wifiConfig , const MP5800_VSG_CONFIG& vsgConfig)
{
	 
	UINT failure = MP5800_ERROR;
	int index = mp5800Index(rfPort);

	if (wifiConfig.Standard == _802_11b) 
	{
	     if ( ( bDebug) || ( bInit) || (mp5800TesterConfigStandard[index] != WIFI_DSSS) )
		{		
				failure = expMV2_SetMeasureMode(rfPort,WIFI_DSSS);
				if ( failure ) 		{
						return MP5800_SET_WIFI_DSSS_ERROR;
				}
			  mp5800TesterConfigStandard[index] = WIFI_DSSS;
		}
	}
	else
	{
	    if ( ( bDebug) || ( bInit) || (mp5800TesterConfigStandard[index] != WIFI_OFDM) )
		{
					
				failure = expMV2_SetMeasureMode(rfPort,WIFI_OFDM);
				if ( failure ) 		{
						return MP5800_SET_WIFI_OFDM_ERROR;
				}
			  mp5800TesterConfigStandard[index] = WIFI_OFDM;
		}
	}

	 mp5800WifiConfig[index].Standard =   wifiConfig.Standard;
	 
	
	failure = expMV2_Set_WIFI_TX_Start(  rfPort , false  );
		 if ( failure)    {
			 return MP5800_SET_WIFI_TX_RUN_ERROR; 
		 }
	   

    failure = expMV2_Set_WIFI_TX( rfPort,
	                            vsgConfig.Frequency_MHz,                      
                                vsgConfig.Output_Power_dBm,
								vsgConfig.Waveforms_Playback.Count,
								wifiConfig.Standard,
								wifiConfig.ChannelBandwidth,
								wifiConfig.DataRate );

	 std::string loadFileString = makeWIFI_WaveformName(wifiConfig);
	 char *LoadFileName = &loadFileString[0u];
	 
	  mp5800VsgConfig[index].Waveforms_Playback.LoadFileName = LoadFileName; 
														   
	 if (  failure )
	 {
		 return MP5800_SET_WIFI_TX_ERROR; 
	 }
	 
	     return MP5800_SUCCESS;	
	   
}
// ---------------------------------------------------------------------------
MP5800_API UINT  MP5800_setWIFI_VSG_Run(  const RF_PORT rfPort, const bool OnOff )
{
	UINT failure = MP5800_ERROR;
	int index = mp5800Index(rfPort);

	 if ( ( bDebug) || ( bInit) || 
	        ( ( mp5800WifiConfig[index].Standard == _802_11b )   &&  ( mp5800TesterConfigStandard[index] != WIFI_DSSS ) ) )
			{  
				 failure = expMV2_SetMeasureMode(rfPort,WIFI_DSSS);
					if ( failure ) 		{
							return MP5800_SET_WIFI_DSSS_ERROR;
					}
				  mp5800TesterConfigStandard[index] = WIFI_DSSS ;
			}
		  

		    if ( ( bDebug) || ( bInit) || 
	        ( ( mp5800WifiConfig[index].Standard != _802_11b )   &&  ( mp5800TesterConfigStandard[index] != WIFI_OFDM ) ) )
			{					
				failure = expMV2_SetMeasureMode(rfPort,WIFI_OFDM);
				if ( failure ) 		{
							return MP5800_SET_WIFI_OFDM_ERROR;
				}
					mp5800TesterConfigStandard[index] = WIFI_OFDM ;
			}
		  
		 
	
	 
	   
	  failure = expMV2_Set_WIFI_TX_Start(  rfPort , OnOff  );
		 if (failure)  
		 {
			 return MP5800_SET_WIFI_TX_RUN_ERROR; 
		 }
	  
	return MP5800_SUCCESS;
		
}



// ---------------------------------------------------------------------------
//  BT RX
// ---------------------------------------------------------------------------
MP5800_API UINT  MP5800_setBT_VSA( const RF_PORT rfPort, const MP5800_BT_CONFIG& btConfig, const MP5800_VSA_CONFIG& vsaConfig)
{
 
UINT failure = MP5800_ERROR ;
	int index = mp5800Index(rfPort);

if ( ( bDebug) || ( bInit) || ( mp5800TesterConfigStandard[index] !=   BLUETOOTH) ) {
		failure = expMV2_SetMeasureMode( rfPort,BLUETOOTH );
		if ( failure ) {
			return MP5800_SET_STANDARD_ERROR;
		}
		mp5800TesterConfigStandard[index] =   BLUETOOTH ;
	}
  

	failure = expMV2_Set_BT_RX( rfPort, 
	                           vsaConfig.Frequency_MHz,                                            
							   vsaConfig.Reference_level_dBm,						             
							   vsaConfig.Setting.Capture_Length_ms*0.001,	       
							   btConfig.PacketType) ;  


	 if (failure)
	 {
		 return MP5800_SET_BT_RX_ERROR; 
	 }
	  failure = MP5800_ERROR;
	 failure = expMV2_SetVsaTriggerType( rfPort, vsaConfig.Setting.Trigger_Type);   
	   if (failure)
	 {
		 return MP5800_SET_BT_RX_ERROR; 
	 }
	 failure = expMV2_SetVsaGapTime( rfPort, vsaConfig.Setting.Gap_Time_us*0.000001);                                          
	  if (failure)
	 {
		 return MP5800_SET_BT_RX_ERROR; 
	 }

	   failure = expMV2_SetVsaTriggerOffsetUs( rfPort, vsaConfig.Setting.Offset_us);                                          
	  if (failure)
	 {
		 return MP5800_SET_BT_RX_ERROR; 
	 }
	     return MP5800_SUCCESS;	

}

MP5800_API UINT  MP5800_getBT_VSA_Value(const RF_PORT rfPort , MP5800_BT_MEASURE& btMeasure)
{
 
	UINT failure = MP5800_ERROR ;
	int index = mp5800Index(rfPort);

	 if ( ( bDebug) || ( bInit) || ( mp5800TesterConfigStandard[index] !=   BLUETOOTH) ) {
		failure = expMV2_SetMeasureMode( rfPort,BLUETOOTH );
		if ( failure ) {
			return MP5800_SET_STANDARD_ERROR;
		}
		mp5800TesterConfigStandard[index] =   BLUETOOTH ;
	}

	 

	failure =  expMV2_Get_BT_RX(  rfPort    ,                    
                               btMeasure.Power_dBm,               
								btMeasure.Initial_Freq_Error_kHz,  
								btMeasure.BDR_Bandwidth_20dB_MHz,  
								btMeasure.BDR_FreqDrift_kHz,       
								btMeasure.BDR_DeltaF1Avg_kHz,      
								btMeasure.BDR_DeltaF2Avg_kHz,      
								btMeasure.BDR_DeltaF2Max_kHz,	   
								btMeasure.EDR_DEVM_percent,            
								btMeasure.EDR_DEVM_Pk_percent,         
								btMeasure.LE_DeltaF1Avg_kHz,       
								btMeasure.LE_DeltaF2Avg_kHz,       
								btMeasure.LE_DeltaF2Max_kHz);

											 
	  if (failure)
	 {
		 return MP5800_GET_BT_RX_VALUE_ERROR; 
	 }
	  else
	 { 
	     return MP5800_SUCCESS;	
	  }
}
// ---------------------------------------------------------------------------
//  BT TX
// ---------------------------------------------------------------------------

MP5800_API UINT  MP5800_setBT_VSG( const RF_PORT rfPort, const MP5800_BT_CONFIG& btConfig, const MP5800_VSG_CONFIG& vsgConfig)
{
 
UINT failure = MP5800_ERROR ;
int index = mp5800Index(rfPort);

	if ( ( bDebug) || ( bInit) || ( mp5800TesterConfigStandard[index] !=   BLUETOOTH) ) {
		failure = expMV2_SetMeasureMode( rfPort,BLUETOOTH );
		if ( failure ) {
			return MP5800_SET_STANDARD_ERROR;
		}
		mp5800TesterConfigStandard[index] =   BLUETOOTH ;
	}
 
	 

	failure = expMV2_Set_WIFI_TX_Start(  rfPort , false  );
	 if (failure)
	 {
			 return MP5800_SET_BT_TX_RUN_ERROR; 
	  }
	 
 
	failure = expMV2_Set_BT_TX( rfPort, 
	                           vsgConfig.Frequency_MHz,                                            
							   vsgConfig.Output_Power_dBm,					             
							   vsgConfig.Waveforms_Playback.Gap_us,
							   vsgConfig.Waveforms_Playback.Count,
							   btConfig.PacketType) ;  

	 std::string loadFileString = makeBT_WaveformName(btConfig);
	 char *LoadFileName = &loadFileString[0u];
	 
	  failure=  expMV2_LoadMemoryWithFilePath(rfPort, LoadFileName, "BT");
	 mp5800VsgConfig[index].Waveforms_Playback.LoadFileName = LoadFileName; 

	 if (failure)
	 {
		 return MP5800_SET_BT_TX_ERROR; 
	 }
	
	     return MP5800_SUCCESS;	
	 
}

// ------------------------------------------------------------------------------
MP5800_API UINT  MP5800_setBT_VSG_Run(  const RF_PORT rfPort , const bool OnOff )
 {
UINT failure = MP5800_ERROR;
 
int index = mp5800Index(rfPort);

if ( ( bDebug) || ( bInit) || ( mp5800TesterConfigStandard[index] !=   BLUETOOTH) ) {
		failure = expMV2_SetMeasureMode( rfPort,BLUETOOTH );
		if ( failure ) {
			return MP5800_SET_STANDARD_ERROR;
		}
		mp5800TesterConfigStandard[index] =   BLUETOOTH ;
	}
 
      
	 
	 
	 
//            result = expMV2_Set_WIFI_TX_Start(  rfPort , false );  // debug
	failure = expMV2_Set_WIFI_TX_Start(  rfPort , OnOff  );
	if (failure)
	{
		return MP5800_SET_BT_TX_RUN_ERROR; 
	}


		failure =  expMV2_Set_4Port_Switch( rfPort,
				  assingPortConfiguration( mp5800TesterConfig[index].Port_Configuration.RF1),
				  assingPortConfiguration( mp5800TesterConfig[index].Port_Configuration.RF2),
				  assingPortConfiguration( mp5800TesterConfig[index].Port_Configuration.RF3),
				  assingPortConfiguration( mp5800TesterConfig[index].Port_Configuration.RF4) ); 
		if (failure) {
			return MP5800_Set_4Port_Switch_FAIL_ERROR;
		}
		 
	return MP5800_SUCCESS;
}

// ---------------------------------------------------------------------------------------------------

MP5800_API UINT  MP5800_saveVSA_WaveformToFile( const RF_PORT rfPort, const char *waveformFileName)
{
	UINT result = MP5800_ERROR;
	result = expMV2_SaveVsaData( rfPort, waveformFileName );
	if ( result ) 
	{
		return MP5800_SAVE_WAVEFORM_TO_FILE_ERROR;
	}
	 return MP5800_SUCCESS;
}


// customized table --------------------------------------------------------------------------
MP5800_API UINT  MP5800_createTable( int tableIndex  )
{
	std::vector<int>::iterator  it;
	it = find ( mp5800TableIndex.begin(), mp5800TableIndex.end(), tableIndex );
	if ( it == mp5800TableIndex.end()) 	{
      	mp5800TableIndex.push_back(tableIndex);
			return MP5800_SUCCESS;
	}
	else
	{ 
		return MP5800_CREATE_CUSTOMIZED_TABLE_ERROR;
	}

}

// ---------------------------------------------------------------------------------------------------
MP5800_API UINT  MP5800_addMeasureItemToTable( int tableIndex , MEASURE_ITEM measureItem )
{
	//1. find table index
	std::vector<int>::iterator  indexIt;
	indexIt = find ( mp5800TableIndex.begin(), mp5800TableIndex.end(), tableIndex );
	if ( indexIt == mp5800TableIndex.end()) 
	{
		return MP5800_ADD_MEASURE_ITEM_ERROR ;
	}
	//2. put test item into table
	std::vector<MEASURE_ITEM>::iterator  it;
	it = find (mp5800MeasureItemIndex[tableIndex].begin(), mp5800MeasureItemIndex[tableIndex].end(), measureItem) ;
	if (it == mp5800MeasureItemIndex[tableIndex].end() ) {
		mp5800MeasureItemIndex[tableIndex].push_back( measureItem);
		return MP5800_SUCCESS;
	}
	else
	{
		return MP5800_ADD_MEASURE_ITEM_ERROR ;
	}
      
	
}
// ------------------------------------------------------------------------------------------------------
MP5800_API UINT  MP5800_getTable( const RF_PORT rfPort, int tableIndex )
{
	UINT failure = MP5800_ERROR ;
	int index = mp5800Index(rfPort);
	double measureValue[30];
	
	mp5800Table[tableIndex].clear();
	bool peakRepeatFlag = false;
	bool fskRepeatFlag = false;
	bool wifiRepeatFlag = false;
	bool btRepeatFlag = false;

	double Freq_Dev_MHz = 0.f;
	double Freq_Offset_MHz = 0.f;
	double Freq_Drift_MHz = 0.f;
	double Dev_ERROR_MHz = 0.f;
	double FSK_ERROR_MHz = 0.f;
														  


	for ( std::vector<MEASURE_ITEM>::iterator measureItem=mp5800MeasureItemIndex[tableIndex].begin(); measureItem!= mp5800MeasureItemIndex[tableIndex].end();measureItem++)

	switch  ( *measureItem )
	{

     case    CHANNEL_POWER_dBm :
						if ( ( bDebug) || ( bInit) || ( mp5800TesterConfigStandard[index] !=   GPRF) ) {
							failure = expMV2_SetMeasureMode( rfPort, GPRF );
							if ( failure ) 	{
								return  MP5800_SET_STANDARD_ERROR;
							}
							mp5800TesterConfigStandard[index] =  GPRF;
						}

	 
						 if ( ( bDebug) || ( bInit) || ( mp5800VsaConfig[index].Demodulation !=   _NULL ) ) {
							failure = expMV2_Set_GPRF_DemodulationType( rfPort, _NULL );
							if ( failure ) {
								return MP5800_SET_DEMODULATION_ERROR;
							}
							 mp5800VsaConfig[index].Demodulation =   _NULL;
						}
	     
						// need to modify
						 if ( !mp5800LoopBack[index] )  {
							failure =expMV2_SetVsaRx_Port(rfPort);
							if( failure) {
								return MP5800_SET_VSA_PORT_ERROR;
							}
						 }

				 
						failure = expMV2_GetPowerByTime(rfPort, measureValue[0]);   
	                    mp5800Table[tableIndex][CHANNEL_POWER_dBm] = measureValue[0];
						if( failure)
						 {
							 mp5800Table[tableIndex][CHANNEL_POWER_dBm]  = MP5800_ERROR_VALUE;
	 
							 return MP5800_ERROR;
						 }

						break;

      case   PEAK_FREQUENCY_MHz :
      case   PEAK_POWER_dBm :						 
		               
						if ( ( bDebug) || ( bInit) || ( mp5800TesterConfigStandard[index] !=   GPRF) ) {
							failure = expMV2_SetMeasureMode( rfPort, GPRF );
							if ( failure ) 	{
								return  MP5800_SET_STANDARD_ERROR;
							}
							mp5800TesterConfigStandard[index] =  GPRF;
						}
							 
						if ( ( bDebug) || ( bInit) || ( mp5800VsaConfig[index].Demodulation !=   _NULL ) ) {
							failure = expMV2_Set_GPRF_DemodulationType( rfPort, _NULL );
							if ( failure ) {
								return MP5800_SET_DEMODULATION_ERROR;
							}
							 mp5800VsaConfig[index].Demodulation =   _NULL;
						}
	     
						// need to modify
						 if ( !mp5800LoopBack[index] )  {
							failure =expMV2_SetVsaRx_Port(rfPort);
							if( failure) {
								return MP5800_SET_VSA_PORT_ERROR;
							}
						 }
 
						if (peakRepeatFlag == false ) 
						{
							failure = expMV2_GetPeakData(rfPort,measureValue[0],measureValue[1]);
							if( failure) {
									return MP5800_GET_PEAK_POWER_FREQUENCY_ERROR;
								}
							peakRepeatFlag = true;
						}

					switch (*measureItem)
					{
				    		case PEAK_FREQUENCY_MHz :					   
							if ( queryTable(failure, tableIndex ,*measureItem,  measureValue[0] ) ) return MP5800_ERROR;
						    break;							
							case PEAK_POWER_dBm :
							if ( queryTable(failure, tableIndex ,*measureItem,  measureValue[1] ) )  return MP5800_ERROR;
							 						 													   			    
					}  break;

	  case FSK_FREQUENCY_DEVIATION_kHz:
	  case FSK_FREQUENCY_OFFSET_kHz:
	  case FSK_FREQUENCY_DRIFT_kHz:
	  case FSK_DEVIATION_ERROR_kHz:
	  case FSK_FSK_ERROR_kHz:				 
		               
						if ( ( bDebug) || ( bInit) || ( mp5800TesterConfigStandard[index] !=   GPRF) ) {
							failure = expMV2_SetMeasureMode( rfPort, GPRF );
							if ( failure ) 	{
								return  MP5800_SET_STANDARD_ERROR;
							}
							mp5800TesterConfigStandard[index] =  GPRF;
						}

	 
						if ( ( bDebug) || ( bInit) || ( mp5800VsaConfig[index].Demodulation !=   FSK ) ) {
							failure = expMV2_Set_GPRF_DemodulationType( rfPort, FSK );
							if ( failure ) {
								return MP5800_SET_DEMODULATION_ERROR;
							}
							 mp5800VsaConfig[index].Demodulation =   FSK;
						}
	     
						// need to modify
						 if ( !mp5800LoopBack[index] )  {
							failure =expMV2_SetVsaRx_Port(rfPort);
							if( failure) {
								return MP5800_SET_VSA_PORT_ERROR;
							}
						 }
						 
						if ( mp5800FskConfig[index].Solution  == Freq_Deviation ){
								 failure = expMV2_GetFSK_FreqDeviation( rfPort , Freq_Dev_MHz );								 
								if( failure) {
									return MP5800_GET_FSK_FREQDEVIATION_ERROR;
								}
								fskMeasure.Freq_Dev_kHz = Freq_Dev_MHz * 1000.;
								mp5800Table[tableIndex][FSK_FREQUENCY_DEVIATION_kHz] = fskMeasure.Freq_Dev_kHz;
								if( failure) {
									 mp5800Table[tableIndex][FSK_FREQUENCY_DEVIATION_kHz] = MP5800_ERROR_VALUE;	 
									 return MP5800_ERROR;
								 }
						}
						else if ( mp5800FskConfig[index].Solution  == All_measurements )
						{
								if (fskRepeatFlag == false ) 
								{ 
										failure = expMV2_GetFSKDemodulation( rfPort ,Freq_Dev_MHz , Freq_Offset_MHz, Freq_Drift_MHz, Dev_ERROR_MHz, FSK_ERROR_MHz );
										if( failure) {
											return MP5800_GET_ALL_MEASUREMENT_ERROR;
										}
										 fskMeasure.Freq_Dev_kHz =  Freq_Dev_MHz * 1000.;
										 fskMeasure.Freq_Offset_kHz = Freq_Offset_MHz * 1000.;
										 fskMeasure.Freq_Drift_kHz = Freq_Drift_MHz * 1000.;
										 fskMeasure.Dev_ERROR_kHz = Dev_ERROR_MHz * 1000.;
										 fskMeasure.FSK_ERROR_kHz = FSK_ERROR_MHz * 1000.;

                                         fskRepeatFlag = true;
								}
								
								switch (*measureItem)
								{
								case  FSK_FREQUENCY_DEVIATION_kHz :
									 if ( queryTable(failure, tableIndex ,*measureItem,  fskMeasure.Freq_Dev_kHz ) ) return MP5800_ERROR;									    
									  break;		
								case  FSK_FREQUENCY_OFFSET_kHz :  	
									  if ( queryTable(failure, tableIndex ,*measureItem,  fskMeasure.Freq_Offset_kHz ) ) return MP5800_ERROR;															  
                                      break;		
								case FSK_FREQUENCY_DRIFT_kHz :
									  if ( queryTable(failure, tableIndex ,*measureItem,  fskMeasure.Freq_Drift_kHz ) ) return MP5800_ERROR;
                                       break;		
								case FSK_DEVIATION_ERROR_kHz :
									   if ( queryTable(failure, tableIndex ,*measureItem,  fskMeasure.Dev_ERROR_kHz ) ) return MP5800_ERROR;    
                                       break;		
								case FSK_FSK_ERROR_kHz :
									    if ( queryTable(failure, tableIndex ,*measureItem,  fskMeasure.FSK_ERROR_kHz ) ) return MP5800_ERROR;  
						
					             }
						}
						
						break;

   //--------------------------------------------------------------------------------------------------------------------------
   
     case WIFI_Power_dBm:
     case WIFI_EVM_dB:
	case  WIFI_Spectrum_Mask_bool:
	case  WIFI_Phase_Error_deg:
	case  WIFI_Freq_Error_kHz:
	case  WIFI_SymClk_Error_ppm:
	case  WIFI_Lo_Leakage_dB:
/*
	case  WIFI_MaskTotalCount:
	case  WIFI_MaskErrorCount:
	case  WIFI_Beamformed:
	case  WIFI_Preamble_Power:
*/                               
		                      // -------------------- DSSS
							  if ( ( bDebug) || ( bInit) || 
 								( ( mp5800WifiConfig[index].Standard == _802_11b )   &&  ( mp5800TesterConfigStandard[index] != WIFI_DSSS ) ) )
								{  
									 failure = expMV2_SetMeasureMode(rfPort,WIFI_DSSS);
										if ( failure ) 	{
												return MP5800_SET_WIFI_DSSS_ERROR;
										}
									  mp5800TesterConfigStandard[index] = WIFI_DSSS ;
								}
		  
							  // -------------------- OFDM
								if ( ( bDebug) || ( bInit) || 
								( ( mp5800WifiConfig[index].Standard != _802_11b )   &&  ( mp5800TesterConfigStandard[index] != WIFI_OFDM ) ) )
								{					
									failure = expMV2_SetMeasureMode(rfPort,WIFI_OFDM);
									if ( failure ) 		{
												return MP5800_SET_WIFI_OFDM_ERROR;
									}
										mp5800TesterConfigStandard[index] = WIFI_OFDM ;
								}	 

								 if ( !mp5800LoopBack[index] )  {
									failure =expMV2_SetVsaRx_Port(rfPort);
									if( failure) {
										return MP5800_SET_VSA_PORT_ERROR;
									}
								 }

							  if (wifiRepeatFlag == false)     {    
 								 failure =  MP5800_getWIFI_VSA_Value(rfPort , wifiMeasure);
								 if( failure) {
												return MP5800_GET_WIFI_MEASUREMENT_ERROR;
								 }
								 wifiRepeatFlag = true;
							 }
						   
                             switch ( *measureItem)
							 {
								 case WIFI_Power_dBm :
									  if ( queryTable(failure, tableIndex ,*measureItem,  wifiMeasure.Power_dBm ) ) return MP5800_ERROR;
                                       break;		
								 case WIFI_EVM_dB :
									   if ( queryTable(failure, tableIndex ,*measureItem,  wifiMeasure.EVM_dB ) ) return MP5800_ERROR;
                                       break;		
                                 case WIFI_Spectrum_Mask_bool :
									   if ( queryTable(failure, tableIndex ,*measureItem,  wifiMeasure.Spectrum_Mask_bool  ) ) return MP5800_ERROR; 
									    break;		
								 case WIFI_Phase_Error_deg :
									   if ( queryTable(failure, tableIndex ,*measureItem,  wifiMeasure.Phase_Error_deg  ) ) return MP5800_ERROR; 
                                        break;		
								 case WIFI_Freq_Error_kHz :
                                        if ( queryTable(failure, tableIndex ,*measureItem, wifiMeasure.Freq_Error_kHz  ) ) return MP5800_ERROR;
                                         break;		
								 case  WIFI_SymClk_Error_ppm :     
                                       if ( queryTable(failure, tableIndex ,*measureItem, wifiMeasure.SymClk_Error_ppm  ) ) return MP5800_ERROR;
									     break;		
								 case  WIFI_Lo_Leakage_dB :
							            if ( queryTable(failure, tableIndex ,*measureItem, wifiMeasure.Lo_Leakage_dB  ) ) return MP5800_ERROR;
							}
/*
		                  if (*i == WIFI_MaskTotalCount)    mp5800Table[tableIndex][*i] = wifiMeasure.MaskErrorCount ;
					      if (*i == WIFI_MaskErrorCount)    mp5800Table[tableIndex][*i] = wifiMeasure.MaskErrorCount ;
						    if (*i == WIFI_Beamformed)    mp5800Table[tableIndex][*i] = wifiMeasure.isBFON ;
						   	   if (*i == WIFI_Preamble_Power)    mp5800Table[tableIndex][*i] = wifiMeasure.PreamblePower;
  */                          
							break;


    case BT_Power_dBm:
	case BT_BDR_Initial_Freq_Error_kHz:
    case BT_BDR_Bandwidth_20dB_MHz:
	case BT_BDR_FreqDrift_kHz:
	case BT_BDR_DeltaF1Avg_kHz:
	case BT_BDR_DeltaF2Avg_kHz:
	case BT_BDR_DeltaF2Max_kHz:
	case BT_EDR_DEVM_percent:
    case BT_EDR_DEVM_Pk_percent:
  	case BT_LE_DeltaF1Avg_kHz:
	case BT_LE_DeltaF2Avg_kHz:
	case BT_LE_DeltaF2Max_kHz:
		                     
		       					if ( ( bDebug) || ( bInit) || ( mp5800TesterConfigStandard[index] !=   BLUETOOTH) ) {
									failure = expMV2_SetMeasureMode( rfPort, BLUETOOTH );
									if ( failure ) 	{
										return  MP5800_SET_STANDARD_ERROR;
									}
									mp5800TesterConfigStandard[index] =  BLUETOOTH;
							 	}


								 if ( !mp5800LoopBack[index] )  {
									failure =expMV2_SetVsaRx_Port(rfPort);
									if( failure) {
										return MP5800_SET_VSA_PORT_ERROR;
									}
								 }

							  if (btRepeatFlag == false)   {    
								 failure =  MP5800_getBT_VSA_Value(rfPort , btMeasure);
								 if( failure) {
												return MP5800_GET_WIFI_MEASUREMENT_ERROR;
								 }
								 btRepeatFlag = true;
							   }
						   
							  switch ( *measureItem)
							  {
									case BT_Power_dBm :
										   if ( queryTable(failure, tableIndex ,*measureItem, btMeasure.Power_dBm  ) ) return MP5800_ERROR;
                                          break;		
									case BT_BDR_Initial_Freq_Error_kHz :
										    if ( queryTable(failure, tableIndex ,*measureItem, btMeasure.Initial_Freq_Error_kHz  ) ) return MP5800_ERROR;
                                          break;		
									case BT_BDR_Bandwidth_20dB_MHz :
						                    if ( queryTable(failure, tableIndex ,*measureItem, btMeasure.BDR_Bandwidth_20dB_MHz  ) ) return MP5800_ERROR;
                                         break;		
									case BT_BDR_FreqDrift_kHz :
								            if ( queryTable(failure, tableIndex ,*measureItem, btMeasure.BDR_FreqDrift_kHz  ) ) return MP5800_ERROR;
                                          break;		
									case BT_BDR_DeltaF1Avg_kHz :
                                            if ( queryTable(failure, tableIndex ,*measureItem, btMeasure.BDR_DeltaF1Avg_kHz   ) ) return MP5800_ERROR;
                                          break;		
									case BT_BDR_DeltaF2Avg_kHz :
										    if ( queryTable(failure, tableIndex ,*measureItem, btMeasure.BDR_DeltaF2Avg_kHz  ) ) return MP5800_ERROR; 
                                           break;		
									case  BT_BDR_DeltaF2Max_kHz :
                                            if ( queryTable(failure, tableIndex ,*measureItem, btMeasure.BDR_DeltaF2Max_kHz  ) ) return MP5800_ERROR; 
                                          break;		
									case BT_EDR_DEVM_percent :
										    if ( queryTable(failure, tableIndex ,*measureItem, btMeasure.BDR_DeltaF2Max_kHz  ) ) return MP5800_ERROR; 
                                           break;		
                                    case BT_EDR_DEVM_Pk_percent :
										    if ( queryTable(failure, tableIndex ,*measureItem, btMeasure.EDR_DEVM_Pk_percent  ) ) return MP5800_ERROR; 
                                           break;		
									case BT_LE_DeltaF1Avg_kHz :
                                            if ( queryTable(failure, tableIndex ,*measureItem, btMeasure.LE_DeltaF1Avg_kHz  ) ) return MP5800_ERROR; 
                                            break;		
									case BT_LE_DeltaF2Avg_kHz :
										    if ( queryTable(failure, tableIndex ,*measureItem, btMeasure.LE_DeltaF2Avg_kHz ) ) return MP5800_ERROR; 
                                            break;		
									case BT_LE_DeltaF2Max_kHz :
										      if ( queryTable(failure, tableIndex ,*measureItem, btMeasure.LE_DeltaF2Max_kHz ) ) return MP5800_ERROR; 
							  }
							break;
                         
	}
 
	 return MP5800_SUCCESS;
}



MP5800_API UINT  MP5800_getMeasureItemFromTable( int tableIndex , MEASURE_ITEM measureItem , double &measureValue )
{
 
		measureValue = mp5800Table[tableIndex][measureItem];

		return MP5800_SUCCESS;

}
