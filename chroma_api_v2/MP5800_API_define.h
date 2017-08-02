#ifndef  MP5800_API_DEFINE_H
#define  MP5800_API_DEFINE_H
//#include <Windows.h>
#include "types.h"
namespace MP5800{
#include "MP5800_API_const.h"
#include "MP5800_Measure_Item.h"
 
/* ---------------------------------------------------------------------------
**  Port proerty  enum 
** --------------------------------------------------------------------------- */

enum RF_PORT { 	RF_PORT_1 = 0, 	RF_PORT_2, 	RF_PORT_3, 	RF_PORT_4, 	RF_PORT_5, 	RF_PORT_6, 	RF_PORT_7, 	RF_PORT_8 };

/* ----------------------------------------------------------------------------
**  Config structure
** ---------------------------------------------------------------------------- */
enum MP5800_SWITCH { SWITCH_OFF, SWITCH_ON, VSA1, VSG1};

#pragma pack(push,1)
struct MP5800_CONFIG
{
    struct _Port_Configuration
    {
		MP5800_SWITCH   			RF1, RF2, RF3, RF4;
		bool 						loadCableLoss;
	}  Port_Configuration;
	 
}   ;
#pragma pack(pop)


enum PORT_MODE { PORT_MODE_OFF, PORT_MODE_RX, PORT_MODE_TX };

enum MP5800_STANDARD { GPRF, WIFI_OFDM,  WIFI_DSSS,  BLUETOOTH , MEASURE_MODE_NULL };


/* ----------------------------------------------------------------------------
**  VSG structure
** ---------------------------------------------------------------------------- */

enum Freq_Band { _2_4G, _5G }  ;
enum Channel { _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14};
enum  MP5800_VSG_MOD { CW, WAVE };
 
#pragma pack(push,1)
struct MP5800_VSG_CONFIG
{
	UINT              				Frequency_MHz;	 
	double            				Output_Power_dBm ; // -90 ~ + 30
	
	struct _Waveforms_Playback
	{
		char*            		    LoadFileName;
		MP5800_SWITCH   		    RF;
		MP5800_VSG_MOD  			MOD;
		UINT  						Count;
		UINT 						Gap_us;
	} Waveforms_Playback;

	struct _Impairments
	{
		int                         CW_Offset_MHz;
	}   Impairments;

}  ;
#pragma pack(pop)

/* ----------------------------------------------------------------------------
**  VSA structure
** ---------------------------------------------------------------------------- */
enum MP5800_VSA_Sampling_Rate  { _5_MHz, _10_MHz, _20_MHz, _40_MHz, _80_MHz, _160_MHz}  ;
enum MP5800_VSA_RBW { _100_Hz, _500_Hz, _1K_Hz, _2K_Hz, _5K_Hz, _10K_Hz, _20K_Hz, _50K_Hz, _100K_Hz, _200K_Hz, _500K_Hz, _1M_Hz, _2M_Hz, _5M_Hz };
enum MP5800_VSA_Window_Type { NO_WINDOW, HAMMING_WINDOW, HANNING_WINDOW, GAUSSION_WINDOW, BLACKMAN_WINDOW, BLACKMAN_HAIS_WINDOW };
enum MP5800_VSA_Trigger_Type { Immediate, Edge_Positive, Edge_Negative, Level, External } ;
enum MP5800_GPRF_Demodulation { _NULL, FSK, PSK, QAM, MSK}  ;



#pragma pack(push,1)
struct MP5800_VSA_CONFIG
{
	
	UINT 							Frequency_MHz;
	MP5800_VSA_Sampling_Rate    	Sampling_Rate;
	double 							Reference_level_dBm ; // -90 ~ + 30

	struct _Setting
	{
		double 						Capture_Length_ms;
		MP5800_VSA_Trigger_Type   	Trigger_Type;
		double 						Threshold_dB;
		UINT 						Gap_Time_us;
		int 						Offset_us;
	 }  Setting;

	struct _RBW
   {
		MP5800_VSA_RBW  			RBW ;
    } RBW;
	MP5800_GPRF_Demodulation Demodulation;
} ;

struct MP5800_VSA_MEASURE
{
     double  Peak_Power_dBm;
     double  Channel_Power_dBm;
	 double  Peak_Frequency_MHz;
};
#pragma pack(pop)

/* ----------------------------------------------------------------------------
**  FSK structure
** ---------------------------------------------------------------------------- */

enum MP5800_FSK_Solution { Freq_Deviation, All_measurements };
enum MP5800_FSK_Filter_Type { NO, RAISED_COSINE, ROOT_RAISED_COSINE, GAUSSIAN};
enum MP5800_FSK_FSK_Type { _2_FSK, _4_FSK, _8_FSK };
enum MP5800_FSK_Symbol_Phase_Coutinuity { Continuous, Discontinuous }; 

#pragma pack(push,1)
struct MP5800_FSK_CONFIG
{ 
			MP5800_FSK_Solution            Solution;
			MP5800_FSK_Filter_Type         Filter_Type;
			MP5800_FSK_FSK_Type            FSK_Type;
	 		MP5800_FSK_Symbol_Phase_Coutinuity                    Symbol_Phase_Coutinuity;
			UINT Filter_Length;
			double BT;
			double Symbol_Rate_MHz;
			double Freq_Dev_KHz;
}  ;

struct MP5800_FSK_MEASURE
{      
        double Freq_Dev_kHz;
        double Freq_Offset_kHz;
        double Freq_Drift_kHz;
        double Dev_ERROR_kHz;
        double FSK_ERROR_kHz;
} ;
#pragma pack(pop) 

 
 
/* ---------------------------------------------------------------------------
**  Wifi enum
** --------------------------------------------------------------------------- */

enum WIFI_FREQUENCY_CORRECTION { OFF_CORRECTION, STF_CORRECTION, LTF_CORRECTION };
enum WIFI_CHANNEL_ESTIMATION {  OFF_ESTIMATION, LTF_ESTIMATION, DATA_ESTIMATION};
enum WIFI_DSSS_FILTER { Rectangular_DSSS, Raise_Consine_DSSS, Root_Raise_Consine_DSSS, Gaussian_DSSS, OFF_DSSS};
enum WIFI_DSS_EVM_METHOD { STANDARD_EVM, STANDARD_2007_EVM, RMS_EVM };

enum WIFI_STANDARD
{
	_802_11ac = 0,
    _802_11n,
    _802_11b,
    _802_11g,
    _802_11a,
} ;

enum  WIFI_BANDWIDTH 
{
	_20M = 0,
    _40M,
    _80M
} ;

enum WIFI_DATA_RATE 
{
    // wifi 11b
	CCK_1M = 1,
	CCK_2M,
	CCK_55M,
	CCK_11M,
		
	// wifi 11a/g
	OFDM_6M,
	OFDM_9M,
	OFDM_12M,
	OFDM_18M,
	OFDM_24M,
	OFDM_36M,
	OFDM_48M,
	OFDM_54M,
		
	// wifi 11ac/n
	MCS0,
	MCS1,
	MCS2,
	MCS3,
	MCS4,
	MCS5,
    MCS6,
	MCS7,
	MCS8,
	MCS9,

} ;


/* ----------------------------------------------------------------------------
**  WIFI structure
** ---------------------------------------------------------------------------- */
#pragma pack(push,1)
struct  MP5800_WIFI_CONFIG
{
     WIFI_STANDARD                 Standard;              
	 WIFI_BANDWIDTH               ChannelBandwidth;      
	 WIFI_DATA_RATE			       DataRate; 

      struct _Correction  // For OFDM
   {
        MP5800_SWITCH                    Phase_Correction;
        MP5800_SWITCH                    Symbol_Clk_Correction;
	    MP5800_SWITCH                    Amplitude_Correction;
        WIFI_FREQUENCY_CORRECTION        Frequency_Correction;
        WIFI_CHANNEL_ESTIMATION           Channel_Estimation;
    } Correction;

    struct _Filter   // for DSSS
    {
       WIFI_DSSS_FILTER       Filter;
	   UINT                   Filter_Length;
	   double                 Filter_Roll_Off;
	   WIFI_DSS_EVM_METHOD    EVM_Method;
    } Filter ;
 };

struct  MP5800_WIFI_MEASURE
{
	  double           Power_dBm;               
	  double           EVM_dB;  
      bool             Spectrum_Mask_bool;                     
	  double           Phase_Error_deg;              
	  double           Freq_Error_kHz;          
	  double           SymClk_Error_ppm;        
	  double           Lo_Leakage_dB;	              
          
};
#pragma pack(pop)

/* ---------------------------------------------------------------------------
**   BT enum
** --------------------------------------------------------------------------- */
enum BT_LE_PAYLOAD_TYPE_DETECTION { Header , Actual_Payload };
enum BT_PACKET_TYPE
{
	// BDR
	 DH1,
	 DH3,
	 DH5,
	// EDR
	 _2DH1,
	 _2DH3,
	 _2DH5,
	 _3DH1,
	 _3DH3,
	 _3DH5,
	// LE
	 LE
};
 



enum BT_STANDARD
{
    BT_BDR = 0,
    BT_EDR,
    BT_LE,
} ;

enum BT_DATA_TYPE
{
	 All_0,
	 All_1,
	 All_10,
	 Random,
	 Four1Four0,
	 PRBS_9,
	 PRBS_15,
	 All_01,
	 Four0Four1
};
/* ---------------------------------------------------------------------------
**   BT struct
** --------------------------------------------------------------------------- */

#pragma pack(push,1)
struct MP5800_BT_CONFIG
{	
	 
    BT_PACKET_TYPE  PacketType;
	BT_DATA_TYPE    DataType;

    
};
 

struct MP5800_BT_MEASURE
{
            double         Power_dBm;                     
			double         Initial_Freq_Error_kHz;          
			double         BDR_Bandwidth_20dB_MHz;                    
			double         BDR_FreqDrift_kHz;                   
			double         BDR_DeltaF1Avg_kHz;
            double         BDR_DeltaF2Avg_kHz;                  
			double         BDR_DeltaF2Max_kHz;	                 
			double		   EDR_DEVM_percent;                        
			double		   EDR_DEVM_Pk_percent;                     
			double         LE_DeltaF1Avg_kHz;                   
			double         LE_DeltaF2Avg_kHz;                   
			double         LE_DeltaF2Max_kHz;      
	
};

 #pragma pack(pop)
 
}

#endif
