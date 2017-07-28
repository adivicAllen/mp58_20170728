#ifndef MFG_TOOL_DEFINE_H
#define MFG_TOOL_DEFINE_H

#include <define.h>
#include <Dut_define.h>
#include <mp5k_define.h>
#include <MFG_INCLUDE.h>

typedef std::string String;
typedef std::wstring wString;

struct _DUT_MAP
{
	static std::multimap<String,String> GetDutMap()         
	{
		std::multimap<String,String> m;           
		m.insert(std::make_pair("BCM43238","BCM43238"));
		m.insert(std::make_pair("MTK7592","MTK7592"));
		m.insert(std::make_pair("MTK7610_7620","MTK7610"));
		m.insert(std::make_pair("MTK7610_7620","MTK7620"));
		m.insert(std::make_pair("MTK7610E","MTK7610E"));		
		m.insert(std::make_pair("MTK7612_7603","MTK7612"));
		m.insert(std::make_pair("MTK7612_7603","MTK7603"));
		m.insert(std::make_pair("MTK7612_7628","MTK7612"));
		m.insert(std::make_pair("MTK7612_7628","MTK7628"));
		m.insert(std::make_pair("MTK7612_7603_MTN","MTK7612"));
		m.insert(std::make_pair("MTK7612_7603_MTN","MTK7603"));		
		m.insert(std::make_pair("MTK7620","MTK7620"));
		m.insert(std::make_pair("MTK7620_MTN","MTK7620_MTN"));
		m.insert(std::make_pair("MTK3662F","MTK3662F"));
		m.insert(std::make_pair("RT5350F","RT5350F"));
		m.insert(std::make_pair("RT5372L","RT5372L"));
		m.insert(std::make_pair("RTL8188EU","RTL8188EU"));
		m.insert(std::make_pair("RTL8188RU","RTL8188RU"));	
		m.insert(std::make_pair("RTL8367RB","RTL8881AB"));
		m.insert(std::make_pair("RTL8367RB","RTL8192ER"));    
		m.insert(std::make_pair("RTL8696","RTL8696"));
		m.insert(std::make_pair("RTL8111au","RTL8111au"));
		m.insert(std::make_pair("RTL8192EU","RTL8192EU"));
		m.insert(std::make_pair("RTL8812au","RTL8812au"));
		m.insert(std::make_pair("RTL9607P","RTL8812E"));
		m.insert(std::make_pair("RTL9607P","RTL8192E"));
		m.insert(std::make_pair("RTL8676S","RTL8676S"));
		m.insert(std::make_pair("RTL8954E_UDP","RTL8814AR"));
		m.insert(std::make_pair("RTL8954E_UDP","RTL8192ER"));
		m.insert(std::make_pair("RTL8954E_TCP","RTL8814AR"));
		m.insert(std::make_pair("RTL8954E_TCP","RTL8192ER"));
		m.insert(std::make_pair("AR9462","AR9462"));
		m.insert(std::make_pair("MTK7620A","MTK7620A"));
		m.insert(std::make_pair("RTL8192ER_HS","RTL8192ER_HS"));
		m.insert(std::make_pair("MTK7620N","MTK7620N"));
		m.insert(std::make_pair("RTL8196E","RTL8192ER"));
		m.insert(std::make_pair("MTK7620A_AsiaRF","MTK7620A_AsiaRF"));
		m.insert(std::make_pair("MTK7612_7628","MTK7612"));
		m.insert(std::make_pair("MTK7612_7628","MTK7628"));
		m.insert(std::make_pair("mp5k_vDUT","mp5k_vDUT"));
		m.insert(std::make_pair("RTL8812AE","RTL8812AE"));
		m.insert(std::make_pair("MTK7688","MTK7688"));
		return m;         
	}
};



typedef enum _GetMethod
{
	USER_MANUAL,
	AUTO_INCREMENT

}GetMethod;

typedef enum _RUNMODE
{
	MULTI			= 0,
	SINGLE			= 1,
	PING_PONG		= 2,
}RUNMODE;

typedef enum _PROCESS
{
	enOpenDUT,
	enCloseDUT,
	enXtalTrim,
	enCalibration,
	enTx,
	enRx,
	enRxSen,
	enWritePowerIndex,
	enWriteMacAddress,
	enBT_Tx,
	enBT_Rx,
	enIdle = 999,
} PROCESS;

typedef struct _MeasureMethod 
{
	bool	isPre_TX;
	bool	isWIFIExtendMeasure;
	bool	isBTExtendMeasure;
}MeasureMethod;

typedef struct _Trim_testitem
{
	UINT		  DUT_No;
	STANDARD      Standard;
	UINT 		  Frequency;
	UINT 		  UseRFPort;
	DUT_ANT	      Ant;
	double		  Cableloss;

} Trim_testitem;

typedef struct _Cal_testitem
{
	UINT		DUT_No;
	STANDARD	Standard;
	UINT		Frequency;
	double		Target_power;
	DUT_ANT		Ant;
	UINT		UseRFPort;
	double		Cableloss;
	double		PowerLimit;
} Cal_testitem;

typedef struct _Tx_testitem
{
	UINT			DUT_No;
	STANDARD		Standard;
	UINT 			Frequency;
	DUT_ANT			Ant;
	UINT 			UseRFPort;

	double			Target_power;
	double 			CaptureLength;
	double			Cableloss;

	bool			doTPC;
	bool			doMask;
	bool			doEVM;
	bool			doBF;
	PREAMBLE		PreambleValue;
	
	//Criteria
	double 			TxPowerUpperLimit;
	double 			TxPowerLowerLimit;
	double			EVMCriteria;
	double			MaskCriteria;
	double			FreqErrorCriteria;

} Tx_testitem;

typedef struct _Rx_testitem
{
	UINT			DUT_No;
	STANDARD		Standard;
	UINT 			Frequency;
	DUT_ANT			Ant;
	double			power;
	bool			doPER;
	UINT 			UseRFPort;
	UINT			PacketCount;
	double			Cableloss;
	double			PER_criteria;
} Rx_testitem;

typedef struct _RxSen_testitem
{
	UINT			DUT_No;
	STANDARD		Standard;
	UINT 			Frequency;
	DUT_ANT			Ant;
	UINT 			UseRFPort;
	UINT			PacketCount;
	double			PER_criteria;
	double			Cableloss;

} RxSen_testitem;

typedef struct _WritePowerIndex_Item
{
	UINT			DUT_No;
	WriteMethod		WriteMe;

}WritePowerIndex_Item;

typedef struct _WriteMac
{
	UINT			DUT_No;
	WriteMethod		WriteMe;
	UINT			InputMethod;
	UINT			IncreaseDelta;

} WriteMac_Item;

typedef struct _OpenDUT
{
	UINT			DUT_No;
	DUTSetting		DUT_Setting;
	char			DUT_SN[MAX_PATH];

} OpenDUT_Item;

typedef struct _CloseDUT
{
	UINT			DUT_No;

} CloseDUT_Item;


/*BT*/
typedef struct _BT_Tx_testitem
{
	//Glo
	UINT			DUT_No;
	BT_TYPE			Standard;
	PACKET_TYPE		Packet_Type;
	UINT 			Frequency;
	double			Cableloss;
	
	//DUT
	DATA_PATTERN	Data_Pattern;
	UINT			Target_Power_Level;
	UINT			PacketLength;

	//MP5K
	UINT 			UseRFPort;
	double			Target_Power_dBm;
	double 			CaptureLength;
	
	//Criteria
	double 			TxPowerUpperLimit;
	double 			TxPowerLowerLimit;

} BT_Tx_testitem;

typedef struct _BT_Rx_testitem
{
	//Glo
	UINT			DUT_No;
	BT_TYPE			Standard;
	PACKET_TYPE		Packet_Type;
	UINT 			Frequency;
	UINT			BitCount;
	double			Cableloss;
	//Dut

	//MP5K	
	UINT 			UseRFPort;
	UINT			DirtyPacket;
	double			Output_Power;

	//Criteria
	double			PER_criteria;
} BT_Rx_testitem;

//-----------------------------------------
typedef struct _RX_Minimum_Sensitivity_Trans
{
	double       rx_min_value;
} RX_Min_Sen_Trans;

typedef struct _EVM_Value_Trans
{
	double       evm_value;
} EVM_value_trans;

typedef struct _Bandwidth_Trans
{
	RX_Min_Sen_Trans rx_min_rate[50];
	EVM_value_trans  evm_rate_trans[50];
} Bandwidth_trans;

typedef struct _Type_Trans
{
	Bandwidth_trans bandwith_trans[3];
} Type_trans;

typedef struct _Tx_Pwr_Lim
{
	double DSSS_Upper;
	double DSSS_Lower;
	double OFDM_Upper;
	double OFDM_Lower;

} Tx_Pwr_Lim;

typedef struct _Mask_Per_Thre
{
	double DSSS;
	double OFDM;
} Mask_per_thre;

typedef struct _Cal_Power_Lim
{
	double DSSS;
	double OFDM;
} Cal_Power_Lim;

typedef struct _Freq_Error_Lim
{
	double DSSS;
	double OFDM;
} Freq_Error_Lim;

typedef struct _Rx_Per_Thre
{
	double DSSS;
	double OFDM;
} Rx_per_thre;

typedef struct _Pass_Criteria
{
	Type_trans		type_trans[5];
	Tx_Pwr_Lim		tx_pwr_value;
	Rx_per_thre		rx_per_value;
	Mask_per_thre	mask_error_value;
	Freq_Error_Lim	Freq_error_value;
	Cal_Power_Lim	Cal_Power_value;

} Pass_Criteria;
//-----------------------------------------
typedef struct _BT_EDR_Value_Trans
{
	String       bt_edr_value;
} BT_EDR_value_Trans;

typedef struct _BT_BR_Value_Trans
{
	String       bt_br_value;
} BT_BR_value_trans;

typedef struct _BT_LE_Value_Trans
{
	String       bt_le_value;
} BT_LE_value_trans;
//-----------------------------------------
typedef struct _BT_Criteria_Value
{
	double			High;
	double			Low;
}BT_Criteria_Value;


typedef struct _BT_EDR_Criteria
{
	double		Power;
	double		InitFreqError;
	double		EVM[2];
	double		BER[2];
} BT_EDR_Criteria;

typedef struct _BT_BDR_Criteria
{
	double				Power;
	double				InitFreqError;
	double				BW_20db[2];
	double				Delta_F2_MAX;
	double				Delta_F1Pk2Pk_Header;
	double				Delta_F2F1_Ratio;
	double				BR_Delta_F1_Pk2Pk;
	double				BER;
	BT_Criteria_Value	FreqDrift[4];
	BT_Criteria_Value	Delta_F1_AVG;

} BT_BDR_Criteria;

typedef struct _BT_LE_Criteria
{
	double					Power;
	double					InitFreqError;
	double					Delta_F2_MAX;
	double					Delta_F2F1_Ratio;
	double					PER;
	BT_Criteria_Value		Delta_F1_AVG;
} BT_LE_Criteria;


typedef struct _BT_Criteria
{
	BT_EDR_Criteria EDR_Criteria;
	BT_BDR_Criteria BDR_Criteria;
	BT_LE_Criteria	LE_Criteria;

} BT_Criteria;

//-----------------------------------------
typedef struct _BT_RATE_PARSING
{
	BT_BR_value_trans  bt_BR_rate[11];
	BT_EDR_value_Trans bt_EDR_rate[14];
	BT_LE_value_trans  bt_LE_rate[10];
} BT_RATE_Parsing;
//--------------------------------------

typedef enum _BT_BR_UI_NAME
{
	BR_Power					= 0,
	BR_Initial_Freq_Error		,
	BR_ICFT_Tail            	,
	BR_Omega_i			   		,
	BR_Bandwidth_20dB       	,
	BR_FreqDrift            	,
	BR_DriftRate_Max        	,
	BR_Delta_F1_Pk2Pk_Header	,
	BR_Delta_F1_Avg        		,
	BR_Delta_F2_Max				,
	BR_BER
}BT_BR_ui_name;

typedef enum _BT_EDR_UI_NAME
{
	EDR_POWER				 = 0,
	EDR_Initial_Freq_Error  	,
	EDR_ICFT_Tail            	,
	EDR_Omega_i              	,
	EDR_Omega_o					,
	EDR_Max_Omerga_io       	,
	EDR_RelativePower        	,
	EDR_DEVM					,
	EDR_DEVM_Pk              	,
	EDR_DEVM99_PASS          	,
	EDR_Delta_F1_Pk2Pk_Header	,
	EDR_Delta_F2_Max_Access		,
	EDR_BER_2M					,
	EDR_BER_3M					,
}BT_EDR_UI_NAME;

typedef enum _BT_LE_UI_NAME
{
	LE_POWER				 = 0,
	LE_Initial_Freq_Offset		,
	LE_Delta_F1_Average			,
	LE_Delta_F2_Max         	,
	LE_Avg_Dev_Sync				,
	LE_F0Fn_Max          		,
	LE_F1F0              		,
	LE_FnFn5_Max				,
	LE_Fn_Max             		,
	LE_PER
}BT_LE_UI_NAME;


//-----------------------------------------------------
class MainTest
{
public:	
	MainTest::MainTest( const Tx_testitem &obj)
	{
		ID = 0;
		type.item_Tx.tx_items = obj;
	};	
	MainTest::MainTest( const Rx_testitem &obj) 
	{
		ID = 1;
		type.item_Rx.rx_items = obj;	
	};
	MainTest::MainTest( const RxSen_testitem &obj) 
	{
		ID = 2;
		type.item_RxSen.rxSen_items = obj;	
	};
	
	MainTest::MainTest(const BT_Tx_testitem &obj)
	{
		ID = 3;
		type.bt_item_Tx.BT_tx_items = obj;
	};
	MainTest::MainTest(const BT_Rx_testitem &obj)
	{
		ID = 4;
		type.bt_item_Rx.BT_rx_items = obj;
	};
	
	Tx_testitem GetTxItem()const{ return type.item_Tx.tx_items; };
	Rx_testitem GetRxItem()const{ return type.item_Rx.rx_items; };
	RxSen_testitem GetRxSenItem()const{ return type.item_RxSen.rxSen_items; };
	BT_Tx_testitem GetBT_TxItem()const{ return type.bt_item_Tx.BT_tx_items; };
	BT_Rx_testitem GetBT_RxItem()const{ return type.bt_item_Rx.BT_rx_items; };

	UINT GetID()const{ return ID; }

private:
	union Mainitem
	{
		struct 
		{
			Tx_testitem  tx_items;
		} item_Tx;
		struct 
		{
			Rx_testitem  rx_items;
		} item_Rx;
		struct 
		{
			RxSen_testitem  rxSen_items;
		} item_RxSen;
		struct
		{
			BT_Tx_testitem  BT_tx_items;
		} bt_item_Tx;
		struct
		{
			BT_Rx_testitem  BT_rx_items;
		} bt_item_Rx;
	};
	UINT ID;
	Mainitem type;
};

typedef struct _General_Setting
{
	bool			XtalTrim;
	bool			Calibration;
	UINT			loop_times;
	UINT			MyID;
	UINT			NextID;
	RUNMODE			RunMode;
	UINT			RetryNum;
	UINT			UsedPort;
	std::vector<DUTExtendSetting>	DUTExtends;

}General_Setting;

typedef struct _MP5KSetting
{
	String						IP;
	MP5K_TriggerConfig			Trigger_para;
	MP5K_CorrectionConfig		Correction_para;

}MP5KSetting;

typedef struct _SN_Setting
{
	
	bool		isRead;
	UINT		SN_Length;
	GetMethod	InputMethod;

} SerialNumber_Setting;

typedef struct _MacAddress_Setting
{
	bool		isRead;
	GetMethod	InputMethod;

} MacAddress_Setting;

typedef struct _AutoPing_Setting
{
	bool	isAutoPing;
	UINT	TimeOut;
	UINT	Ping_Count;
	UINT	ExtRetry;

}AutoPing_Setting;

typedef struct _MES_Setting
{
	bool		isRead;
	String		FilePath;
	wString		WSName;
	wString		Pre_WSName;
	wString		WS_ID;
	wString		PC_ID;
	wString		WorkList;
	wString		CustomerName;
	String		ServerIP;

} MES_Setting;

typedef struct _MFG_Profile
{

	//Setting
	General_Setting							GL_Setting;
	std::vector<DUTSetting>					Dut_Settings;
	std::vector<MP5KSetting>				MP5K_Settings;
	
	//TEST ITEM	
	std::vector<Trim_testitem>				Trim_Items;
	std::vector<Cal_testitem>				Cal_Items;
	std::vector<MainTest>					Main_Items;
	
} MFG_Profile;

#endif