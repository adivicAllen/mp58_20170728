// Demo3650.cpp : Defines the entry point for the console application.
// vcDemo.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <curses.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#define WINDOWS_LEAN_AND_MEAN
#include "types.h"
#include <string>
#include  <map>
using namespace std;
#define CUSTOMIZED_TABLE    1

ostream *outFile;

enum{
GPRF_TABLE,
FSK_TABLE,
DSSS_TABLE,
OFDM_TABLE,
BDR_TABLE,
EDR_TABLE,
LE_TABLE
};
 

//#pragma comment(lib, "..\\MP5800_API.lib")
#include "MP5800_API.h"
map <BT_PACKET_TYPE, std::string>  btPacketType;
map <BT_DATA_TYPE,  std::string>   btDataType; 
map <WIFI_STANDARD, std::string>    wifiStandard;
map <WIFI_BANDWIDTH, std::string>  wifiChannelBandwidth;
map <WIFI_DATA_RATE , std::string > wifiDataRate;


MP5800_CONFIG       testerConfig;

  // config
MP5800_VSG_CONFIG   vsgConfig;
MP5800_VSA_CONFIG   vsaConfig;
MP5800_FSK_CONFIG   fskConfig;
MP5800_WIFI_CONFIG  wifiConfig;
MP5800_BT_CONFIG    btConfig;
  //
MP5800_VSA_MEASURE    vsaMeasure;
MP5800_FSK_MEASURE    fskMeasure;
MP5800_WIFI_MEASURE   wifiMeasure;
MP5800_BT_MEASURE     btMeasure;
UINT failure, result;

void initMap()
{
	btPacketType.insert(pair<BT_PACKET_TYPE, std::string>  (  DH1, "BT_DH1" ));
	btPacketType.insert(pair<BT_PACKET_TYPE, std::string>  ( DH3, "BT_DH3" ));
	btPacketType.insert(pair<BT_PACKET_TYPE, std::string>  ( DH5, "BT_DH5" ));
	btPacketType.insert(pair<BT_PACKET_TYPE, std::string>  ( _2DH1, "BT_2DH-1" ));
	btPacketType.insert(pair<BT_PACKET_TYPE, std::string>  ( _2DH3, "BT_2DH-3" ));
	btPacketType.insert(pair<BT_PACKET_TYPE, std::string>  ( _2DH5, "BT_2DH-5" ));
	btPacketType.insert(pair<BT_PACKET_TYPE, std::string>   ( _3DH1, "BT_3DH-1" ));
	btPacketType.insert(pair<BT_PACKET_TYPE, std::string>   ( _3DH3, "BT_3DH-3" ));
	btPacketType.insert(pair<BT_PACKET_TYPE, std::string>   ( _3DH5, "BT_3DH-5" ));
	btPacketType.insert(pair<BT_PACKET_TYPE, std::string>   ( LE, "BT_LE" ));

	btDataType.insert(pair<BT_DATA_TYPE,  std::string> ( All_0, "_All 0" ));
	btDataType.insert(pair<BT_DATA_TYPE,  std::string> ( All_1, "_All 1" ));
	btDataType.insert(pair<BT_DATA_TYPE,  std::string> ( All_10, "_Alt 10" ));
	btDataType.insert(pair<BT_DATA_TYPE,  std::string> ( Random, "_Random" ));
	btDataType.insert(pair<BT_DATA_TYPE,  std::string> ( Four1Four0, "_11110000" ));
	btDataType.insert(pair<BT_DATA_TYPE,  std::string> ( PRBS_9, "_PRBS-9" ));
	btDataType.insert(pair<BT_DATA_TYPE,  std::string> ( PRBS_15, "_PRBS-15" ));
	btDataType.insert(pair<BT_DATA_TYPE,  std::string> ( All_01, "_Alt 01" ));
	btDataType.insert(pair<BT_DATA_TYPE,  std::string> ( Four0Four1, "_00001111" ));

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

UINT CWTestSub(int i)
{
      vsgConfig.Frequency_MHz = 2402 ;
      vsgConfig.Output_Power_dBm = -10 * i;
      vsgConfig.Waveforms_Playback.LoadFileName = "BER_gfsk_120M_192ms_no_header.wfm";
      vsgConfig.Waveforms_Playback.MOD = CW;
      vsgConfig.Impairments.CW_Offset_MHz = 2;
	  vsgConfig.Waveforms_Playback.Gap_us = 0;
      vsaConfig.Setting.Capture_Length_ms = 1;
      vsaConfig.Frequency_MHz = vsgConfig.Frequency_MHz ;                 
      vsaConfig.Reference_level_dBm =  vsgConfig.Output_Power_dBm;
      vsaConfig.RBW.RBW =  _2K_Hz;   
  //    vsaConfig.RBW.RBW =  _500_Hz;  // _500_Hz;
      vsaConfig.Sampling_Rate = _10_MHz;
      vsaConfig.Setting.Trigger_Type = Immediate    ;
  
      // control

    //  result = MP5800_setVSG( RF_PORT_1, vsgConfig );// 0 means continuous
      cout << "MP5800_setVSG :" << result << endl;
      if (result) { return result; }

      result = MP5800_setVSG_Run( RF_PORT_1, true );
      cout << "MP5800_setVSG_Run :" << result << endl;
      if (result) { return result; }

      result = MP5800_setVSA( RF_PORT_4, vsaConfig ) ;
      cout << "MP5800_setVSA :" << result << endl;
      if (result) { return result; }

      result =  MP5800_getVSA_Value( RF_PORT_4, vsaMeasure);
      cout << " MP5800_getVSA_Value:" << result << endl;
      if (result) { return result; }

      cout << "CW output power : " <<  vsgConfig.Output_Power_dBm  << endl;
      cout << "CW measured Power : " <<  vsaMeasure.Channel_Power_dBm  << endl;
	  cout << "CW measured peak Power : " <<  vsaMeasure.Peak_Power_dBm  << endl;
	  cout << "CW measured peak frequency   : " <<  vsaMeasure.Peak_Frequency_MHz  << endl;

	 cout<<"========================= GPRF table test ==========================" << endl; 
     result = MP5800_getTable(RF_PORT_4, GPRF_TABLE );
     cout << "MP5800_getTable(GPRF_TABLE ):" << result << endl;
     if (result) { return result; }
	 	    
     double measureValue;
     result = MP5800_getMeasureItemFromTable( GPRF_TABLE, CHANNEL_POWER_dBm , measureValue);
     cout << "CHANNEL_POWER_dbm  : " <<  measureValue  << endl;
	 result = MP5800_getMeasureItemFromTable( GPRF_TABLE, PEAK_POWER_dBm , measureValue);
	 cout << "PEAK_POWER_dBm  : " <<  measureValue  << endl;
	 result = MP5800_getMeasureItemFromTable( GPRF_TABLE, PEAK_FREQUENCY_MHz, measureValue);
	 cout << "PEAK_FREQUENCY_MHz   : " <<  measureValue  << endl;

   

       return MP5800_SUCCESS;
}

UINT  FSKTestSub(int i)
{
   vsgConfig.Frequency_MHz = 2410 ;	  
   vsaConfig.Frequency_MHz = vsgConfig.Frequency_MHz ;        
  vsgConfig.Output_Power_dBm = -10*i;   
  vsgConfig.Waveforms_Playback.MOD = WAVE;
  vsaConfig.Reference_level_dBm =  vsgConfig.Output_Power_dBm;
  vsaConfig.Sampling_Rate = _20_MHz;
  vsgConfig.Impairments.CW_Offset_MHz = 0;
  fskConfig.Solution = Freq_Deviation;
  fskConfig.Filter_Type = GAUSSIAN;
  fskConfig.FSK_Type = _2_FSK;
  fskConfig.Symbol_Phase_Coutinuity =  Continuous;
  fskConfig.Filter_Length = 6;
  fskConfig.BT = 0.5;
  fskConfig.Symbol_Rate_MHz = 1.0;
  fskConfig.Freq_Dev_KHz = 250;
 
 
  result = MP5800_setVSG( RF_PORT_1, vsgConfig ); 
  cout << "MP5800_setVSG :" << result << endl;
  if (result) { return result; }

  result = MP5800_setVSG_Run( RF_PORT_1, true );
  cout << "MP5800_setVSG_Run :" << result << endl;
  if (result) { return result; }
 
  result = MP5800_setFSK_VSA( RF_PORT_4 ,fskConfig ,vsaConfig );
  cout << "MP5800_setFSK_VSA_Value:" << result << endl; 
  if (result) { return result; }

   result = MP5800_getFSK_VSA_Value( RF_PORT_4, fskMeasure);
   cout << "MP5800_getFSK_VSA_Value:" << result << endl; 
   if (result) { return result; }
   cout << "RF_PORT_4 Frequency  Frequency deviation : " << fskMeasure.Freq_Dev_kHz << endl;
 
  // ------------------- set all measurement ----------------------
  fskConfig.Solution = All_measurements;
  result = MP5800_setFSK_VSA( RF_PORT_4 ,fskConfig ,vsaConfig );
  cout << "MP5800_setFSK_VSA_Value:" << result << endl; 
  if (result) { return result; }
  
   
  result = MP5800_getFSK_VSA_Value( RF_PORT_4, fskMeasure);
  cout << "MP5800_getFSK_VSA_Value:" << result << endl; 
  if (result) { return result; }  
   cout << "RF_PORT_4 Frequency  Frequency deviation (kHz) : " <<  fskMeasure.Freq_Dev_kHz << endl;  
   cout << "RF_PORT_4 Frequency  Frequency offset (kHz):" << fskMeasure.Freq_Offset_kHz <<endl;   
   cout << "RF_PORT_4 Frequency  Frequency drift (kHz): "<< fskMeasure.Freq_Drift_kHz<<endl;   
   cout << "RF_PORT_4 Frequency  Frequency DevError (kHz): "<< fskMeasure.Dev_ERROR_kHz << endl; 
   cout << "RF_PORT_4 Frequency  Frequenc  Fsk Error deviation (kHz): " <<  fskMeasure.FSK_ERROR_kHz << endl;

	cout<<"========================= FSK table test ==========================" << endl; 
     result = MP5800_getTable(RF_PORT_4, FSK_TABLE );
     cout << "MP5800_getTable(FSK_TABLE ):" << result << endl;
     if (result) { return result; }

     double measureValue;
     result = MP5800_getMeasureItemFromTable( FSK_TABLE, CHANNEL_POWER_dBm , measureValue);
     cout << "CHANNEL_POWER_dbm  : " <<  measureValue  << endl;

	 result = MP5800_getMeasureItemFromTable( FSK_TABLE, PEAK_POWER_dBm , measureValue);
	 cout << "PEAK_POWER_dBm  : " <<  measureValue  << endl;

	 result = MP5800_getMeasureItemFromTable( FSK_TABLE, PEAK_FREQUENCY_MHz, measureValue);
	 cout << "PEAK_FREQUENCY_MHz   : " <<  measureValue  << endl;

	 result = MP5800_getMeasureItemFromTable( FSK_TABLE, FSK_FREQUENCY_DEVIATION_kHz, measureValue);
	 cout << "FSK_FREQUENCY_DEVIATION_kHz   : " <<  measureValue  << endl;

	 result = MP5800_getMeasureItemFromTable( FSK_TABLE, FSK_FREQUENCY_OFFSET_kHz, measureValue);
	 cout << "FSK_FREQUENCY_OFFSET_kHz   : " <<  measureValue  << endl;
	 
	 result = MP5800_getMeasureItemFromTable( FSK_TABLE, FSK_FREQUENCY_DRIFT_kHz, measureValue);
	 cout << "FSK_FREQUENCY_DRIFT_kHz   : " <<  measureValue  << endl;

	 result = MP5800_getMeasureItemFromTable( FSK_TABLE, FSK_DEVIATION_ERROR_kHz, measureValue);
	 cout << "FSK_DEVIATION_ERROR_kHz   : " <<  measureValue  << endl;

	   result = MP5800_getMeasureItemFromTable( FSK_TABLE, FSK_FSK_ERROR_kHz, measureValue);
	 cout << "FSK_FSK_ERROR_kHz   : " <<  measureValue  << endl;

	cout<<"===================================================="<<endl;
     
      return MP5800_SUCCESS;
}



INT  WifiTestSub(int WifiStandard, int WifiChannelBandWidth , int  WifiDataRate)
{
    cout<<"=====================WIFI ======================"<<endl;
    UINT result;
 

     vsgConfig.Frequency_MHz = 5500;
	  vsaConfig.Frequency_MHz = vsgConfig.Frequency_MHz;
     vsgConfig.Output_Power_dBm = -10.;
     vsgConfig.Waveforms_Playback.Count = 0;
     vsgConfig.Waveforms_Playback.MOD = WAVE;
    
     wifiConfig.Standard = (WIFI_STANDARD) WifiStandard ;
     wifiConfig.ChannelBandwidth = (WIFI_BANDWIDTH ) WifiChannelBandWidth;
	 wifiConfig.DataRate = (WIFI_DATA_RATE) WifiDataRate ;
	 cout << "standard :" << wifiStandard[wifiConfig.Standard] << ", "
	      << "bandwidth :" <<wifiChannelBandwidth[wifiConfig.ChannelBandwidth]<< " ," 
		   << "Data_Rate:" << wifiDataRate[wifiConfig.DataRate]<<endl;
	 
    
     vsaConfig.Reference_level_dBm = vsgConfig.Output_Power_dBm;
	 if (wifiConfig.Standard == _802_11b )
	 {
        vsaConfig.Setting.Capture_Length_ms = 5;
    //    vsaConfig.Setting.Trigger_Type = Immediate;
		vsaConfig.Setting.Trigger_Type = Edge_Positive;
	 }
	 else
	 {
		 vsaConfig.Setting.Capture_Length_ms = 3;
		 vsaConfig.Setting.Trigger_Type = Edge_Positive;
	 }
      
    

     vsaConfig.Setting.Gap_Time_us = 10;
     vsaConfig.Setting.Threshold_dB = -20.;
	 vsaConfig.Setting.Offset_us =  0;  // for MP5800 standard waveform
  //    vsaConfig.Setting.Offset_us =  -5; 

     wifiConfig.Correction.Frequency_Correction = LTF_CORRECTION ;
     wifiConfig.Correction.Channel_Estimation = DATA_ESTIMATION ;

     result = MP5800_setWIFI_VSG( RF_PORT_1, wifiConfig , vsgConfig );
	 cout << "MP5800_setWIFI_VSG :"<< result << endl;
	 if (result) {return result ;}
   
     result = MP5800_setWIFI_VSG_Run(  RF_PORT_1 ,  true );
     cout << "MP5800_setWIFI_VSG_Run :"<< result << endl;
	 if (result) {return result ;}

      result =  MP5800_setWIFI_VSA(  RF_PORT_4, wifiConfig, vsaConfig );
     cout << "MP5800_setWIFI_VSA :"<< result << endl;
	 if (result) {return result ;}

     result =  MP5800_getWIFI_VSA_Value( RF_PORT_4, wifiMeasure);
     cout << "MP5800_getWIFI_VSA_Value :"<< result << endl;
	 if (result) {return result ;}

     cout<< "wifi Power (dbm)  ........" << wifiMeasure.Power_dBm << endl ;
     cout<< "wifi EVM (db) ..............." <<  wifiMeasure.EVM_dB << endl ;
     cout << "wifi Spetrum Mask (pass/Fail)  ............." << wifiMeasure.Spectrum_Mask_bool << endl;
	 cout << "wifi Phase Error (deg) ........" << wifiMeasure.Phase_Error_deg << endl;
	 cout << "wifi Frequency error (kHz) ...." << wifiMeasure. Freq_Error_kHz<< endl;
     cout << "wifi symbol colcok error (ppm) .." << wifiMeasure.SymClk_Error_ppm << endl;
	 cout << "wifi Lo leakage (dB) ........." << wifiMeasure. Lo_Leakage_dB  << endl;


	 if   (  wifiConfig.Standard == _802_11b )
	 {
	     cout<<"========================= DSSS table test ==========================" << endl; 
   
		 result = MP5800_getTable(RF_PORT_4, DSSS_TABLE );
		 double measureValue;
	  
		 result = MP5800_getMeasureItemFromTable( DSSS_TABLE, WIFI_Power_dBm, measureValue);
		 cout << "WIFI_Power_dBm   : " <<  measureValue  << endl;

		 result = MP5800_getMeasureItemFromTable( DSSS_TABLE, WIFI_EVM_dB, measureValue);
		 cout << "WIFI_EVM_dB   : " <<  measureValue  << endl;
	 
		 result = MP5800_getMeasureItemFromTable( DSSS_TABLE, WIFI_Spectrum_Mask_bool, measureValue);
		 cout << "WIFI_Spectrum_Mask_bool   : " <<  measureValue  << endl;

		 result = MP5800_getMeasureItemFromTable( DSSS_TABLE, WIFI_Phase_Error_deg, measureValue);
		 cout << "WIFI_Phase_Error_deg   : " <<  measureValue  << endl;

		 result = MP5800_getMeasureItemFromTable( DSSS_TABLE, WIFI_Freq_Error_kHz, measureValue);
		 cout << "WIFI_Freq_Error_kHz   : " <<  measureValue  << endl;

		 result = MP5800_getMeasureItemFromTable( DSSS_TABLE, WIFI_SymClk_Error_ppm, measureValue);
		 cout << "WIFI_SymClk_Error_ppm   : " <<  measureValue  << endl;

		 result = MP5800_getMeasureItemFromTable( DSSS_TABLE, WIFI_Lo_Leakage_dB, measureValue);
		 cout << "WIFI_Lo_Leakage_dB  : " <<  measureValue  << endl;

		  cout << endl;
	 }

	 else   
	 {
	     cout<<"========================= OFDM table test ==========================" << endl; 
     
	    result = MP5800_getTable(RF_PORT_4, OFDM_TABLE );
	    double measureValue;
	 
		  result = MP5800_getMeasureItemFromTable( OFDM_TABLE , WIFI_Power_dBm, measureValue);
		  cout << "WIFI_Power_dBm   : " <<  measureValue  << endl;
	  
		  result = MP5800_getMeasureItemFromTable( OFDM_TABLE , WIFI_EVM_dB, measureValue);
		  cout << "WIFI_EVM_dB   : " <<  measureValue  << endl;
	 
		  result = MP5800_getMeasureItemFromTable( OFDM_TABLE , WIFI_Spectrum_Mask_bool, measureValue);
		  cout << "WIFI_Spectrum_Mask_bool   : " <<  measureValue  << endl;

		  result = MP5800_getMeasureItemFromTable( OFDM_TABLE , WIFI_Phase_Error_deg, measureValue);
		  cout << "WIFI_Phase_Error_deg   : " <<  measureValue  << endl;
	  
		  result = MP5800_getMeasureItemFromTable( OFDM_TABLE , WIFI_Freq_Error_kHz, measureValue);
		  cout << "WIFI_Freq_Error_kHz   : " <<  measureValue  << endl;

		   result = MP5800_getMeasureItemFromTable( OFDM_TABLE , WIFI_SymClk_Error_ppm, measureValue);
		   cout << "WIFI_SymClk_Error_ppm   : " <<  measureValue  << endl;

		   result = MP5800_getMeasureItemFromTable( OFDM_TABLE , WIFI_Lo_Leakage_dB, measureValue);
		   cout << "WIFI_Lo_Leakage_dB  : " <<  measureValue  << endl;

			 cout << endl;
	 }


     return MP5800_SUCCESS;

}

UINT  BTTestSub(int BTPacketType, int BtDataType)
{
	 
 
	 vsgConfig.Frequency_MHz = 2441;
	 vsaConfig.Frequency_MHz = vsgConfig.Frequency_MHz;
	 vsgConfig.Output_Power_dBm = -10.;
	 vsgConfig.Waveforms_Playback.Count = 0;
	 vsgConfig.Waveforms_Playback.Gap_us = 625;
	 vsgConfig.Waveforms_Playback.MOD = WAVE;
	 
	 btConfig.PacketType = (BT_PACKET_TYPE) BTPacketType;
	 btConfig.DataType = (BT_DATA_TYPE) BtDataType;
	 vsaConfig.Setting.Trigger_Type = Edge_Positive   ;
	
	 vsaConfig.Reference_level_dBm = -10.;
	// vsaConfig.Setting.Offset_us = -10;
	 vsaConfig.Setting.Offset_us = 0;    // for MP5800 standard waveform
	 /*
	  if ((   btConfig.PacketType <= DH5 ) ||  (   btConfig.PacketType == _2DH5 )  || (   btConfig.PacketType == _3DH5 ) )// BDR
	  {
         vsaConfig.Setting.Capture_Length_ms = 5;
	  }
	  else
	  {
		  vsaConfig.Setting.Capture_Length_ms = 3;
	  }
	  */
	   vsaConfig.Setting.Capture_Length_ms = 5;
	 vsaConfig.Setting.Gap_Time_us = 10;

	  cout<< "BT Packet Type: " + btPacketType[btConfig.PacketType] + "; BT Data Type: " +  btDataType[btConfig.DataType]  << endl;

	  result = MP5800_setBT_VSG( RF_PORT_1, btConfig, vsgConfig);
	  cout<< "MP5800_setBT_VSG : "<< result << endl;
	  if (result) { return result; }  
	 
      result = MP5800_setBT_VSG_Run( RF_PORT_1 , true ); 	 
	  cout << "MP5800_setBT_VSG_Run : " <<  result << endl;
	   if (result) { return result; }  
	   
      result =  MP5800_setBT_VSA( RF_PORT_4, btConfig, vsaConfig );
	  cout << "MP5800_setBT_VSA : " <<  result << endl;
	  if (result) { return result; }  
	    	 
	  result=  MP5800_getBT_VSA_Value( RF_PORT_4 , btMeasure);
	  cout << "MP5800_getBT_VSA_Value: " <<  result << endl;
	  if (result) { return result; }  
	
	  cout << "bt Power (dBm) .........." << btMeasure.Power_dBm << endl;
     
	   // =================================================================================================
	   if (   btConfig.PacketType <= DH5 )  // BDR
	  {
		    cout << "bt bandwidth at -20 db (MHz) ........" << btMeasure.BDR_Bandwidth_20dB_MHz<< endl;
		    cout << "bt initial Frequency Error (kHz) .." << btMeasure.Initial_Freq_Error_kHz << endl;

		    if ( ( btConfig.DataType ==  Four1Four0 ) || ( btConfig.DataType ==  Four0Four1 ) )// F1
		    {
		         cout << "BDR_DeltaF1Avg (kHz) ........" << btMeasure.BDR_DeltaF1Avg_kHz << endl;
		    }

		    if ( ( btConfig.DataType ==  All_01 ) || ( btConfig.DataType ==  All_10 ) )   // F2
		    {
				  cout << "BDR_FreqDrift (kHz)........." << btMeasure.BDR_FreqDrift_kHz << endl;
				  cout << "BDR_DeltaF2Avg (kHz) ........" << btMeasure.BDR_DeltaF2Avg_kHz << endl;	 
				  cout << "BDR_DeltaF2Max (kHz)........." << btMeasure.BDR_DeltaF2Max_kHz << endl;
		     }

			 //----------------------------------------------------
             cout<<"======================== BDR table test =========================" << endl; 
     
	         result = MP5800_getTable(RF_PORT_4, BDR_TABLE );
	         double measureValue;

			 result = MP5800_getMeasureItemFromTable( BDR_TABLE  , BT_Power_dBm, measureValue);
	         cout << "BT_Power_dBm : " <<  measureValue  << endl;
			  result = MP5800_getMeasureItemFromTable( BDR_TABLE  , BT_BDR_Bandwidth_20dB_MHz, measureValue);
	         cout << "BT_BDR_Bandwidth_20dB_MHz : " <<  measureValue  << endl;
		      result = MP5800_getMeasureItemFromTable( BDR_TABLE  , BT_BDR_Initial_Freq_Error_kHz, measureValue);
	         cout << "BT_Initial_Freq_Error_kHz : " <<  measureValue  << endl;

	         if ( ( btConfig.DataType ==  Four1Four0 ) || ( btConfig.DataType ==  Four0Four1 ) )// F1
             {
				 result = MP5800_getMeasureItemFromTable( BDR_TABLE  , BT_BDR_DeltaF1Avg_kHz, measureValue);
				 cout << "BT_BDR_DeltaF1Avg_kHz : " <<  measureValue  << endl;
				 cout << endl;
	         }

			if ( ( btConfig.DataType ==  All_01 ) || ( btConfig.DataType ==  All_10 ) )   // F2
			{
				result = MP5800_getMeasureItemFromTable( BDR_TABLE  , BT_BDR_FreqDrift_kHz, measureValue);
				cout << "BT_BDR_FreqDrift_kHz : " <<  measureValue  << endl;
				result = MP5800_getMeasureItemFromTable( BDR_TABLE  , BT_BDR_DeltaF2Avg_kHz, measureValue);
				cout << "BT_BDR_DeltaF2Avg_kHz : " <<  measureValue  << endl;
				result = MP5800_getMeasureItemFromTable( BDR_TABLE, BT_BDR_DeltaF2Max_kHz, measureValue);
				cout << "BT_BDR_DeltaF2Max_kHz : " <<  measureValue  << endl;
				cout << endl;
			}
	     
	  }
	 	 
      // ============================================================================================
	   if ( ( btConfig.PacketType >= _2DH1 ) &&  ( btConfig.PacketType <= _3DH5 ) )  // EDR
	  {
			if ( ( btConfig.DataType ==  PRBS_9 ) ||  ( btConfig.DataType ==  All_0 ) || ( btConfig.DataType ==  All_1 )
				  ||  ( btConfig.DataType ==  All_01 ) ||  ( btConfig.DataType ==  All_10 ) )
			{
				 cout << "EDR_DEVM (%).............." << btMeasure.EDR_DEVM_percent << endl;
				 cout << "EDR_DEVMPeak (%) .........." << btMeasure.EDR_DEVM_Pk_percent << endl;
			}

			// --------------------------- EDR TABLE --------------------------------
			 cout<<"======================== EDR table test =========================" << endl; 
			 result = MP5800_getTable(RF_PORT_4, EDR_TABLE );
	         double measureValue;                
	 
			  result = MP5800_getMeasureItemFromTable( EDR_TABLE  , BT_Power_dBm, measureValue);
	         cout << "BT_Power_dBm : " <<  measureValue  << endl;

			 if ( ( btConfig.DataType ==  PRBS_9 ) ||  ( btConfig.DataType ==  All_0 ) || ( btConfig.DataType ==  All_1 )
				  ||  ( btConfig.DataType ==  All_01 ) ||  ( btConfig.DataType ==  All_10 ) )
			{
				 result = MP5800_getMeasureItemFromTable( EDR_TABLE  , BT_EDR_DEVM_percent, measureValue);
				 cout << "BT_EDR_DEVM_percent : " <<  measureValue  << endl;
				 result = MP5800_getMeasureItemFromTable( EDR_TABLE  , BT_EDR_DEVM_Pk_percent, measureValue);
				 cout << "BT_EDR_DEVM_Pk_percent : " <<  measureValue  << endl;
				 cout << endl;
		    }
	 }

     // ----------------------------------- LE ---------------------------------------

	  if (  btConfig.PacketType == LE )  // LE
	 {
		  if ( ( btConfig.DataType ==  Four1Four0 ) || ( btConfig.DataType ==  Four0Four1 ) )  // F1
		  {
	        cout << "LE_DeltaF1Avg (kHz) .........." << btMeasure.LE_DeltaF1Avg_kHz << endl;
		  }
		   
           if ( ( btConfig.DataType ==  All_01 ) || ( btConfig.DataType ==  All_10 ) )
		  {
		     cout << "LE_DeltaF2Avg (kHz)........." << btMeasure.LE_DeltaF2Avg_kHz << endl;
		     cout << "LE_DeltaF2Max (kHz)........." << btMeasure.LE_DeltaF2Max_kHz << endl;
		  }

		    // -------------------------- LE TABLE ----------------------------------
		   cout<<"======================== LE table test =========================" << endl; 

		      result = MP5800_getTable(RF_PORT_4, LE_TABLE );
	         double measureValue;                
	 
			  result = MP5800_getMeasureItemFromTable( LE_TABLE  , BT_Power_dBm, measureValue);
	         cout << "BT_Power_dBm : " <<  measureValue  << endl;

             if ( ( btConfig.DataType ==  Four1Four0 ) || ( btConfig.DataType ==  Four0Four1 ) )// F1
             {
				 result = MP5800_getMeasureItemFromTable(  LE_TABLE   , BT_LE_DeltaF1Avg_kHz, measureValue);
				 cout << "BT_LE_DeltaF1Avg_kHz : " <<  measureValue  << endl;
				 cout << endl;
	         }
			 

			if ( ( btConfig.DataType ==  All_01 ) || ( btConfig.DataType ==  All_10 ) )   // F2
			{ 
				result = MP5800_getMeasureItemFromTable(  LE_TABLE  , BT_LE_DeltaF2Avg_kHz, measureValue);
				cout << "BT_LE_DeltaF2Avg_kHz : " <<  measureValue  << endl;
				result = MP5800_getMeasureItemFromTable(  LE_TABLE  , BT_LE_DeltaF2Max_kHz, measureValue);
				cout << "BT_LE_DeltaF2Max_kHz : " <<  measureValue  << endl;
				cout << endl;
			}
	 }
	 cout << "=======================================   BT =================================" << endl;
 		 
 
	 return MP5800_SUCCESS;

}

void AdivicTest()
{
	fstream file1;
	  
	//outFile = new(ostream);  .open("..\\RF_LOGS\\data.txt");
   // ================== initial ================
   initMap();
    result = MP5800_connect( "..\\InitConfigFile.ini", true);
     cout << "MP5800_connect :" <<  result << endl ;
    if (result) { exit(0); }

    testerConfig.Port_Configuration.RF1 =   VSG1;
    testerConfig.Port_Configuration.RF2  =  SWITCH_OFF;
    testerConfig.Port_Configuration.RF3  =  SWITCH_OFF;
    testerConfig.Port_Configuration.RF4  =  VSA1;
    testerConfig.Port_Configuration.loadCableLoss = false;


    //1. ----------- build table

    result = MP5800_createTable( GPRF_TABLE );
    result = MP5800_addMeasureItemToTable( GPRF_TABLE, CHANNEL_POWER_dBm ) ;
	result = MP5800_addMeasureItemToTable( GPRF_TABLE, PEAK_POWER_dBm ) ;
	result = MP5800_addMeasureItemToTable( GPRF_TABLE, PEAK_FREQUENCY_MHz ) ;


    result = MP5800_createTable( FSK_TABLE);
    result = MP5800_addMeasureItemToTable( FSK_TABLE, CHANNEL_POWER_dBm ) ;
    result = MP5800_addMeasureItemToTable( FSK_TABLE, PEAK_POWER_dBm ) ;
	result = MP5800_addMeasureItemToTable( FSK_TABLE, PEAK_FREQUENCY_MHz ) ;
    result = MP5800_addMeasureItemToTable( FSK_TABLE, FSK_FREQUENCY_DEVIATION_kHz) ;
	result = MP5800_addMeasureItemToTable( FSK_TABLE, FSK_FREQUENCY_OFFSET_kHz) ;
	result = MP5800_addMeasureItemToTable( FSK_TABLE, FSK_FREQUENCY_DRIFT_kHz) ;
	result = MP5800_addMeasureItemToTable( FSK_TABLE, FSK_DEVIATION_ERROR_kHz) ;
	result = MP5800_addMeasureItemToTable( FSK_TABLE, FSK_FSK_ERROR_kHz) ;

	result = MP5800_createTable( DSSS_TABLE);
    
    result = MP5800_addMeasureItemToTable( DSSS_TABLE, WIFI_Power_dBm) ;
	result = MP5800_addMeasureItemToTable( DSSS_TABLE, WIFI_EVM_dB) ;
	result = MP5800_addMeasureItemToTable( DSSS_TABLE, WIFI_Spectrum_Mask_bool) ;
	result = MP5800_addMeasureItemToTable( DSSS_TABLE, WIFI_Phase_Error_deg) ;
	result = MP5800_addMeasureItemToTable( DSSS_TABLE, WIFI_Freq_Error_kHz) ;
	result = MP5800_addMeasureItemToTable( DSSS_TABLE, WIFI_SymClk_Error_ppm) ;
	result = MP5800_addMeasureItemToTable( DSSS_TABLE, WIFI_Lo_Leakage_dB) ;

	
	result = MP5800_createTable( OFDM_TABLE);
  
    result = MP5800_addMeasureItemToTable( OFDM_TABLE, WIFI_Power_dBm) ;
	result = MP5800_addMeasureItemToTable( OFDM_TABLE, WIFI_EVM_dB) ;
	result = MP5800_addMeasureItemToTable( OFDM_TABLE, WIFI_Spectrum_Mask_bool) ;
	result = MP5800_addMeasureItemToTable( OFDM_TABLE, WIFI_Phase_Error_deg) ;
	result = MP5800_addMeasureItemToTable( OFDM_TABLE, WIFI_Freq_Error_kHz) ;
	result = MP5800_addMeasureItemToTable( OFDM_TABLE, WIFI_SymClk_Error_ppm) ;
	result = MP5800_addMeasureItemToTable( OFDM_TABLE, WIFI_Lo_Leakage_dB) ;

	result = MP5800_createTable(BDR_TABLE);
  
    result = MP5800_addMeasureItemToTable( BDR_TABLE, BT_Power_dBm) ;
	result = MP5800_addMeasureItemToTable( BDR_TABLE, BT_BDR_Initial_Freq_Error_kHz) ;
	result = MP5800_addMeasureItemToTable( BDR_TABLE, BT_BDR_FreqDrift_kHz) ;
	result = MP5800_addMeasureItemToTable( BDR_TABLE, BT_BDR_Bandwidth_20dB_MHz) ;
	result = MP5800_addMeasureItemToTable( BDR_TABLE, BT_BDR_DeltaF1Avg_kHz) ;
	result = MP5800_addMeasureItemToTable( BDR_TABLE, BT_BDR_DeltaF2Avg_kHz) ;
	result = MP5800_addMeasureItemToTable( BDR_TABLE, BT_BDR_DeltaF2Max_kHz) ;


	result = MP5800_createTable(EDR_TABLE);
    
    result = MP5800_addMeasureItemToTable( EDR_TABLE, BT_Power_dBm) ;
	 
	result = MP5800_addMeasureItemToTable( EDR_TABLE, BT_EDR_DEVM_percent) ;
	result = MP5800_addMeasureItemToTable( EDR_TABLE, BT_EDR_DEVM_Pk_percent) ;


	result = MP5800_createTable(LE_TABLE);
   
    result = MP5800_addMeasureItemToTable(LE_TABLE, BT_Power_dBm) ;	 
	result = MP5800_addMeasureItemToTable( LE_TABLE, BT_LE_DeltaF1Avg_kHz) ;
	result = MP5800_addMeasureItemToTable( LE_TABLE, BT_LE_DeltaF2Avg_kHz) ;
	result = MP5800_addMeasureItemToTable( LE_TABLE, BT_LE_DeltaF2Max_kHz) ;

	 

    //2. ----------- set tester

     result = MP5800_setConfig(RF_PORT_1, testerConfig);
     cout << "MP5800_setConfig :" <<  result << endl ;
    if (result) { exit(0); }
	
   for (int ii=0;ii<100000;ii++)
   {
	    
		for(int i=0;i<3;i++)
		{
			  if  ( CWTestSub(i)   )
			  {
				  result = MP5800_disconnect();
				  system("pause"); break;
			  }
		//   system("pause");

		}
//	 	system("pause");

		 for(int i=0;i<3;i++)
		{
			  if  ( FSKTestSub(i)   )
			  {
				  result = MP5800_disconnect();
				  system("pause"); break;
			  }
		//  system("pause");
		}
	 	//system("pause");

		// 11 B ---
		for (int k = CCK_1M ; k<= CCK_11M ; k++)
		{
		   if( WifiTestSub(_802_11b,_20M,k)  )
		   {
			   result = MP5800_disconnect();
              system("pause"); break;
		   }
		  //   system("pause");
		}

		// 11 a
		
		for (int k = OFDM_6M ; k<= OFDM_54M ; k++)
		{
		   if( WifiTestSub(_802_11a,_20M,k)  )
		   {
			   result = MP5800_disconnect();
               system("pause"); break;
		   }
		    //  system("pause");
		}
		

		 
		// 11 g
		for (int k = OFDM_6M ; k<= OFDM_54M ; k++)
		{
		   if( WifiTestSub(_802_11g,_20M,k)  )
		   {
			   result = MP5800_disconnect();
               system("pause"); break;
		   }
		   //  system("pause");
		}
 

		// 11 n
		for( int j = _20M ; j <= _40M ; j++)
		{
			for (int k = MCS0 ; k<= MCS7  ; k++)
			{
			   if( WifiTestSub(_802_11n,j,k)  )
			   {
				   result = MP5800_disconnect();
				  system("pause"); break;
			   }
			 //    system("pause");
			}
		}

		WIFI_DATA_RATE wifiMCS;
			// 11 ac
		for( int j = _20M ; j <= _80M ; j++)
		{
		    if( j == _20M ) 
			{
				 wifiMCS = MCS8;
			}
			else
			{
				 wifiMCS = MCS9;
			}
			
					for (int k = MCS0 ; k<= wifiMCS  ; k++)
					{
					   if( WifiTestSub(_802_11ac,j,k)  )
					   {
						   result = MP5800_disconnect();
						  system("pause"); break;
					   }
					}
			 
			 
		}
   // system("pause");
    
     // ======================BDR ===================
    for( int i= DH1 ; i<= DH5; i++ )
	{
		 for (int j = All_0 ; j < Four0Four1 ; j++)
		 {
			 if ( ( j != Random ) && ( j != PRBS_15) )
			 {
				 if( BTTestSub( i,j ) )
					{
						result = MP5800_disconnect();
						system("pause"); 
						break;
					}
				  // system("pause");
			 }
		 }
	}
	  // ====================== EDR ===================
	 for( int i= _2DH1 ; i<= _3DH5; i++ )
	{
		  for (int j = All_0 ; j < Four0Four1 ; j++)
		 {
			 if ( ( j != Random ) && ( j != PRBS_15 ) )
			 {
				 if( BTTestSub( i,j ) )
					{
						result = MP5800_disconnect();
						system("pause");
						break;
					}
				//   system("pause");
			 }
		 }
	}

     // ====================== LE ===================

	  int  i = LE;
        for (int j = All_0 ; j < Four0Four1 ; j++)
       {
		   if (  ( j != PRBS_15 ) && ( j != Random ) && ( j!= All_0 ) && ( j!= All_1) )
            {
                if( BTTestSub( i,j )    )
                {
                    result = MP5800_disconnect();
					 system("pause"); 
                    break;
					 
                }
             }
         }   
   }
}

/*
int _tmain(int argc, _TCHAR* argv[])
{
	AdivicTest();
	system("pause");
	return 0;
}
*/


