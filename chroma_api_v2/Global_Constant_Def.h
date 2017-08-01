#ifndef _GLOBAL_CONSTANT_DEF_
#define _GLOBAL_CONSTANT_DEF_
#include <QString>
#include <atlstr.h>

//Constant define
#define ConfigFileExtendName	"cig"
#define TestplanExtendName		"pln"
#define SpecFileExtendName		"spe"

#define MP5800_Loss_ExtendName	"tbl"
#define MP5800_Compare			"csv"


#define MAX_STRING_LEN			1014
#define MP5800_Low_Freq			50	  //MHz
#define MP5800_High_Freq		6000  //MHz

#define MAX_FREQUENCY			6000  //MHz
#define MIN_FREQUENCY			10	  //MHz
#define FREQUENCY_STEP			10	  //MHz
#define TABLE_FREQ_SIZE			((MAX_FREQUENCY-MIN_FREQUENCY)/FREQUENCY_STEP)+1

#define CAL100USB2SPI_INDEX		0
#define CAL100_PORT_NUM			8
#define CAL100_LB_PATH_NUM		4
#define CAL100_DataCheckSumLength 8
#define MAX_PW_LEVEL			30	  //dbm
#define MIN_PW_LEVEL			-10   //dbm
#define PW_LEVEL_STEP			5	  //dbm
#define TABLE_PW_SIZE			((MAX_PW_LEVEL-MIN_PW_LEVEL)/PW_LEVEL_STEP)+1;
#define CAL100_Start_Tag		"Cal100Index"
#define CAL100_CalPW_STD_Level		0 //0 
#define Agilent_PM_Freq_Table_Name	"CUSTOM_A"
#define Agilent_PM_Offset_MAX		80


#define Cal_PWR_INDEX			1
#define CalMeaSleep				300			//300
#define SG_FirstSleep			5000		//1000
#define SA_MeaSleep				1000
#define SA_SetFreqSleep			50
#define NS_Ref_Interval			5

//#define NS_ATT UI setting					30
#define CableLoss_Min	0.1
#define CableLoss_Max	20
#define CableLoss_Step	0.2
//NS Cable loss Table
#define NS_LOSS_MAX	50
//BT_test
#define BT_DUT_COM_MAX	2
//MP5800_CalBox
#define MP5800_PORT_NUM 4
#define MP5800_CW_Sleep			200
#define MP5800_CW_Init_Sleep	100

typedef enum
{
	M1_Port1,
	M1_Port2,
	M1_Port3,
	M1_Port4,
	M2_Port1,
	M2_Port2,
	M2_Port3,
	M2_Port4,
	None,
}MP5800_Port;


typedef enum
{
	MP_1,
	MP_2,
	MP_3,
	MP_4,
}MP5800_INSTR;

typedef enum
{
	ADVIC_MP5800,
	ADVIC_MP5K,
	ADVIC_MP7600,
	INSTR_SG,
	RS_SA_FSP_13,
	AG_SA_E4407B,
	ANRITSU_ML2437A,
	Agilent_PM,
	Agilent_N5222A,
	Fake_Instr,
	INSTR_Def_Num //Must define at last
}Remote_INSTR;  //Support Remote INSTR

typedef struct 
{
	QString qsTestItem;
 	double	fH_spec;
	double 	fL_spec;
 	double	f_Value;
	QString qsDiscription;
	USHORT usErrorCode;
}LogMessageFormat;

typedef struct{
	double		_dref_off_level;
	double		_dref_on_level_plus;
	double		_drbw_mhz;
	double		_span_mhz;
	double		_sweep_time;
	USHORT		_us_avg_count;
	double		_dchanel_bandwith;
}SA_Setting;

typedef struct{
	double fFrequency;
	double fTxLoss[CAL100_PORT_NUM];
	double fRxLoss[CAL100_PORT_NUM];
}FreqLossPoint;

typedef struct{
	double fFrequency;
	double fCableLoss;
}SingleCableLossPoint;

typedef struct{
	double fFrequency;
	double fPortLoss[CAL100_PORT_NUM];
}Box_PM_LossPoint;

typedef struct{
	double fFrequency;
	double fPortLoss[CAL100_LB_PATH_NUM];
}Box_Loopback_LossPoint;

#endif //_GLOBAL_CONSTANT_DEF_
 