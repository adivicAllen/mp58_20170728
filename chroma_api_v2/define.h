#ifndef _DEFINE_H_
#define _DEFINE_H_
//#include <Windows.h>
#include <vector>
#include "types.h"

#define DUTLOG 1
#define MP5KLOG 1

#define DBG_ENTRY()
#define DBG(format)
#define DBG_RETURN()


#define SAFE_DELETE(p)			do { if(p){ delete (p); (p) = nullptr; }} while(0)
#define SAFE_DELETE_ARRAY(p)	do { if(p){ delete[] (p); (p) = nullptr; }} while(0)

#define MP5000_API extern "C" __declspec(dllexport)

typedef enum _VoidDataType
{
	_INT = 0,
	_UINT,
	_Double,
	_String,
	_WString,
	_Char,
	_WChar,
	_Bool
}VoidDataType;

typedef enum _TYPE 
{
        _802_11AC = 0,
        _802_11N,
        _802_11B,
        _802_11G,
        _802_11A,
		_unknowTYPE = 99
} TYPE;
 
typedef enum _BANDWIDTH 
{
        _20M = 0,
        _40M,
        _80M
} BANDWIDTH;
 
typedef enum _ANT
{
	ANT_A	= 1,
	ANT_B	,
	ANT_C	,
	ANT_D	,
	ANT_AB	,
	ANT_AC	,
	ANT_AD	,
	ANT_BC	,
	ANT_BD	,
	ANT_CD	,
	ANT_ABC	,
    ANT_ABD	,
    ANT_ACD	,
    ANT_BCD	,
    ANT_ABCD	
}DUT_ANT;

typedef enum _RATE 
{
        //11b
		CCK_1M = 1,
		CCK_2M,
		CCK_55M,
		CCK_11M,
		
		//11a/g
		OFDM_6M,
		OFDM_9M,
		OFDM_12M,
		OFDM_18M,
		OFDM_24M,
		OFDM_36M,
		OFDM_48M,
		OFDM_54M,
		
		//11ac/n
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
		MCS10,
		MCS11,
		MCS12,         
		MCS13,
		MCS14,
		MCS15,
		MCS16,
		MCS17,         
		MCS18,
		MCS19,
		MCS20,
		MCS21,
		MCS22,         
		MCS23,
		MCS24,
		MCS25,
		MCS26,
		MCS27,         
		MCS28,
		MCS29,
		MCS30,
		MCS31,
		unKnowRate = 999
} RATE;

typedef struct _MODULATION
{
	BANDWIDTH Bandwidth;
	RATE rate;
} MODULATION;

typedef struct _STANDARD
{
	TYPE type;			
	bool IsGap;
		
	union 
	{
		struct 
		{
			MODULATION Modulation;
		}OFDM;
		struct 
		{
			RATE rate;
		}DSSS;

	};
}STANDARD;

static const UINT Measure_Channel[132]= {2402,2403,2404,2405,2406,2407,2408,2409,2410,2411,2412,2413,2414,2415,2416,2417,2418,2419,2420,2421,2422,2423,2424,2425,2426,
	2427,2428,2429,2430,2431,2432,2433,2434,2435,2436,2437,2438,2439,2440,2441,2442,2443,2444,2445,2446,2447,2448,2449,2450,2451,2452,2453,2454,2455,2456,2457,2458,
	2459,2460,2461,2462,2463,2464,2465,2466,2467,2468,2469,2470,2471,2472,2473,2474,2475,2476,2477,2478,2479,2480,2484,5180,5190,5200,5210,5220,5230,5240,5250,5260,5270,
	5280,5290,5300,5310,5320,5500,5510,5520,5530,5540,5550,5560,5570,5580,5590,5600,5610,5620,5630,5640,5650,5660,5670,5680,5690,5700,5710,5745,5755,5765,5775,
	5785,5795,5805,5815,5825,5835,5845,5855,5865,5875,5885};

typedef enum _TRIGGER_TYPE
{
	TRIGGER_IMMEDIATE,
	TRIGGER_POSTIVE,
	TRIGGER_NEGATIVE,
	TRIGGER_LEVEL
}TRIGGER_TYPE;

//-----------------BT Structure--------------------------

typedef enum _BT_TYPE 
{
    _BT_BDR = 0,
    _BT_EDR,
    _BT_LE,

} BT_TYPE;

typedef enum _DATA_PATTERN
{
	ALL_ZERO,
	ALL_ONE,
	ZOZO,
	FOFO,
	PN9,
	PN15,
	OFOF,
	OZOZ

}DATA_PATTERN;

typedef enum _PACKET_TYPE
{
	DM1,
	DH1,
	DM3,
	DH3,
	DM5,
	DH5,
	_2DH1,
	_2DH3,
	_2DH5,
	_3DH1,
	_3DH3,
	_3DH5,
	LE

}PACKET_TYPE;

#endif
