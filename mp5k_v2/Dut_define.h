#ifndef _DUT_DEFINE_H_
#define _DUT_DEFINE_H_

#include <define.h>
#define DUT_TOTAL_CHANNEL		26

typedef enum _ConnectMode
{
	_Telnet,
	_Serial,
	_USB,
	_ADB,
	_PCIe
}ConnectMode;


typedef enum _PREAMBLE
{
    Long_Preamble	= 0x01,
    Short_Preamble  	  ,
    Long_GI     		  ,
    Short_GI  		
} PREAMBLE;


typedef enum _WriteMethod
{
	NoWrite,
	PassOnly,
	AlwaysWrite

}WriteMethod;


typedef struct _DUTExtendSetting
{
	WriteMethod	MacMode;
	WriteMethod	PowerMode;
	bool isTrim;
	bool isCalibration;
	
}DUTExtendSetting;


typedef struct _DUTSetting
{
	char	DUT_Model[MAX_PATH];
	char	DUT_SN[MAX_PATH];
	ConnectMode	Mode;
	DUTExtendSetting Extend;

}DUTSetting;

typedef struct _CableLossGoldenDataParsing
{
	bool				MeasuredCableloss_bool;
	UINT				TargetPower;
	UINT 				GoldentFrq[DUT_TOTAL_CHANNEL];
	UINT 				Goldentbl[DUT_TOTAL_CHANNEL];
	UINT				Antenna;
	std::string			MChannel;
	std::string			dataName;
}MDataParsing;

typedef struct DUT__TriggerConfig
{
	TRIGGER_TYPE TriggerType;
	double TriggerThreshold; // Level
	double TriggerGapTime;   // SliceTime
	double TriggerOffset;
}DUT_TriggerConfig;

typedef struct _DutAttribution
{
	std::string DutName;

	UINT rbw;
	bool IsXtalTrim;
	bool IsPowerCalibration;
	bool gap;
	bool Caltrigger;
	bool Xtaltrigger;
	int  sz_DumpCal;
	int  sz_CalInfo;
	DUT_TriggerConfig Tri_config;
	double Trim_reference;
	double Sen_reference;
	double Rx_GapTime;
}DutAttr;

//Crystal Trim struct
typedef struct _DUT_XtalTrimConfig
{
	_DUT_XtalTrimConfig(UINT _Freq,DUT_ANT _ANT)
	{
		Ant				= _ANT;
		Frequency		= _Freq;
	};

	UINT 		  Frequency;
	DUT_ANT	      Ant;

} DUT_XtalTrimConfig;

//Power Calibration struct

typedef struct _DUT_CalConfig
{
	_DUT_CalConfig(STANDARD _Sta,UINT _Freq,double _target_power,DUT_ANT _ANT,double _Cableloss)
	{
		Ant				= _ANT;
		Standard		= _Sta;
		Frequency		= _Freq;
		Target_power	= _target_power;
		Cableloss		= _Cableloss;
	}
	
	STANDARD      Standard;
	UINT 		  Frequency;	
    double 		  Target_power;	
	DUT_ANT	      Ant;
	double		  Cableloss;
} DUT_CalConfig;

//Tx Verification struct
typedef struct _DUT_TxConfig
{
	_DUT_TxConfig( STANDARD _Sta,UINT _Freq,double _power,double _Capture,DUT_ANT _ANT,PREAMBLE _Preamble,double _Cableloss,bool _isBF, bool _isBFTX)
	{
		Standard		= _Sta;
		Frequency		= _Freq;
		Target_power	= _power;
		CaptureLength	= _Capture;
		Ant				= _ANT;		
		PreambleType	= _Preamble;
		Cableloss		= _Cableloss;
		isBeamforming	= _isBF;
		isBFTX			= _isBFTX;
	};
	
	STANDARD		Standard;
	UINT 			Frequency;
	double			Target_power;
	double			CaptureLength;
	DUT_ANT			Ant;
	PREAMBLE		PreambleType;
	double			Cableloss;
	bool			isBeamforming;
	bool			isBFTX;			
} DUT_TxConfig;

//Rx Verification struct
typedef struct _DUT_RxConfig
{
	_DUT_RxConfig(STANDARD _sta, UINT _Freq, DUT_ANT _Ant,double _Cableloss)
	{
		Ant			= _Ant;
		Standard	= _sta;
		Frequency	= _Freq;
		Cableloss	= _Cableloss;
	};

	STANDARD      Standard;
	UINT 		  Frequency;	
	DUT_ANT 	      Ant;
	double		  Cableloss;
} DUT_RxConfig;


typedef struct _DUT_SenConfig
{
	_DUT_SenConfig(STANDARD _Sta,UINT _Freq,DUT_ANT _ANT,double _PerReq)
	{
		Standard  = _Sta;
		Frequency = _Freq;
		Ant		  = _ANT;
		PER		  = _PerReq;
	};
	
	STANDARD		Standard;
	UINT 			Frequency;	
	DUT_ANT			Ant;
	double			PER;
	//double			Cableloss;
} DUT_SenConfig;


typedef struct _DUT_MultiTaskConfig
{	
	char*	NewIP;
	char*	OriginalIP;
	UINT	ID;
	
}DUT_MultiTaskConfig;
//--------------------BT Structure----------------------
//Tx Verification struct
typedef struct _BT_DUT_TxConfig
{
	_BT_DUT_TxConfig( PACKET_TYPE _Sta,UINT _Freq, DATA_PATTERN _Pattern, UINT _power,UINT _PacketLength )
	{
		Standard		= _Sta;
		Frequency		= _Freq;
		Pattern			= _Pattern;
		Target_power	= _power;
		PacketLength    = _PacketLength;
	};
	
	PACKET_TYPE		Standard;
	UINT 			Frequency;
	DATA_PATTERN    Pattern;
	UINT			Target_power;
	UINT			PacketLength;

} BT_DUT_TxConfig;

//Rx Verification struct
typedef struct _BT_DUT_RxConfig
{
	_BT_DUT_RxConfig(PACKET_TYPE _sta, UINT _Freq ,UINT _PacketCount )
	{
		Standard	= _sta;
		Frequency	= _Freq;
		PacketCount = _PacketCount;
	};

	PACKET_TYPE   Standard;
	UINT 		  Frequency;	
	UINT		  PacketCount;
} BT_DUT_RxConfig;

#endif