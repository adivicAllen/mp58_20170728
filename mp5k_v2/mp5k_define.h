#ifndef _MP5K_DEFINE_H_
#define _MP5K_DEFINE_H_

#include "define.h"

typedef enum _FREQUENCY_CORRECTION
{
	FREQ_CORR_OFF,
	FREQ_CORR_STF,
	FREQ_CORR_LTF

}MP5K_Frequency_Correction;

typedef enum _CHANNEL_ESTIMATION
{
	CHANNEL_ESTIMATION_OFF,
	CHANNEL_ESTIMATION_LTF,
	CHANNEL_ESTIMATION_DATA

}MP5K_Channel_Estimation;

typedef struct MP5K_TriggerConfig
{
	TRIGGER_TYPE TriggerType;
	double TriggerThreshold; // Level
	double TriggerGapTime;   // SliceTime
	double TriggerOffset;

}MP5K_TriggerConfig;

typedef struct _CorrectionConfig
{
	MP5K_Frequency_Correction	FrequencyCorrection;
	MP5K_Channel_Estimation		ChannelEstimation;

}MP5K_CorrectionConfig;

typedef struct _XtalTrimConfig
{
	UINT UseRFPort;
	STANDARD Standard;
	UINT FreqMHz;
	UINT RBW; // Hz
	double Cableloss;
	double Trim_reference;

}MP5K_XtalTrimConfig;

typedef struct _CalConfig
{
	UINT UseRFPort;
	STANDARD Standard;
	UINT FreqMHz;
	double RefLeveldBm;

}MP5K_CalConfig;

typedef struct _TxTestConfig
{
	UINT UseRFPort;
	STANDARD Standard;
	UINT FreqMHz;
	double RefLeveldBm;
	double CaptureLength; //ms
	bool isBeamforming;

}MP5K_TxTestConfig;

typedef struct _TxTestMeas
{
	double TXPowerdBm;        // return tx power
	double EVM;               // return evm
	bool   MASK;			// return mask

	double PhaseError;        // return phase error
	double FrequencyError;    // return frequency error
	double SymbolClockError;  // return symbol clock error
	double LoLeakage;	       // return lo leakage
	double MaskTotalCount;
	double MaskErrorCount;
	
	bool  isBFON;
	double PreamblePower;


}MP5K_TxTestMeas;

typedef struct _RxTestConfig
{
	UINT UseRFPort;
	UINT PacketCount;
	STANDARD Standard;
	UINT FreqMHz; // add
	double OutPowerdBm; // output

}MP5K_RxTestConfig;

typedef struct _SpectrumMaskData
{
	double Margin;		//db
	double FreqOffset;	//Mhz
	
}MP5K_SpectrumMaskData;

typedef struct _TxTestMeasExtend
{
	union 
	{
		struct 
		{
			bool Flatness;
			MP5K_SpectrumMaskData	Lower4;
			MP5K_SpectrumMaskData	Lower3;
			MP5K_SpectrumMaskData	Lower2;
			MP5K_SpectrumMaskData	Lower1;
			MP5K_SpectrumMaskData	Upper4;
			MP5K_SpectrumMaskData	Upper3;
			MP5K_SpectrumMaskData	Upper2;
			MP5K_SpectrumMaskData	Upper1;
		}OFDM;
		struct 
		{
			bool RampOn;
			bool RampOff;
			MP5K_SpectrumMaskData	Lower2;
			MP5K_SpectrumMaskData	Lower1;
			MP5K_SpectrumMaskData	Upper2;
			MP5K_SpectrumMaskData	Upper1;
		}DSSS;
	};	

}MP5K_TxTestMeasExtend;

//BlueTooth

typedef struct _BT_TxTestConfig
{
	UINT			UseRFPort;
	PACKET_TYPE		Type;
	UINT			FreqMHz;
	double			RefLeveldBm;
	double			CaptureLength; //ms

}MP5K_BT_TxTestConfig;

typedef struct _BT_TxTestMeas
{
	union	TypeValue
	{
		struct
		{
			double BT_DEVM;
			double BT_DEVMPeak;
		}BT_EDR;
		
		struct
		{
			double	BW_20db;
			double	DeltaF1Avg;
			double	DeltaF2Avg;
			double	DeltaF2Max;
			double	FreqDrift;
		}BT_BDR;
		
		struct
		{
			double	DeltaF1Avg;
			double	DeltaF2Avg;
			double	DeltaF2Max;
		}BT_LE;
	};
	
	double		BT_InitFreq_Error;
	double		BT_TargetPower;        // return tx power
	TypeValue	BT_TX_ReturnValue;

}MP5K_BT_TxTestMeas;

typedef struct _BT_ACP
{
	double	Offset_L1;
	double	Offset_L2;
	double	Offset_L3;
	double	Offset_L4;
	double	Offset_L5;
	double	Offset_0;
	double	Offset_R1;
	double	Offset_R2;
	double	Offset_R3;
	double	Offset_R4;
	double	Offset_R5;
}BT_ACPData;

typedef struct _BT_TxTestMeasExtend
{
	BT_ACPData	ACP_Data;
	bool		ACP_Check;

}MP5K_BT_TxTestMeasExtend;

typedef struct _BT_RxTestConfig
{
	UINT			UseRFPort;
	UINT			BitCount;
	PACKET_TYPE		Type;

	UINT			FreqMHz; // add
	double			OutPowerdBm; // output
	double			GapTime;

}
MP5K_BT_RxTestConfig;
//

//--//
typedef enum _Rx_Start_Mode
{
	RX_START_SYNC,
	RX_START_ASYNC

}MP5K_RX_Start_Mode;


//-- GPS --//
typedef enum _GPS_Data_Pattern
{
	GPS_NVDATA,
	GPS_ZOZO,
	
}GPS_Data_Pattern;


typedef struct _GPS_SingleCh_Config
{
	double				OutputPower;
	double				DopplerHz;
	UINT				SVID;
	GPS_Data_Pattern	DataPattern;
	
}GPS_SingleCh_Config;


//CableLoss
typedef struct _MP5K_Cableloss_Data
{
	UINT	Frequency;
	UINT	Port;

	double Cableloss;

}MP5K_Cableloss_Data;

#endif