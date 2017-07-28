#ifndef _MP5K_ERROR_H_
#define _MP5K_ERROR_H_

#include "mp5k_define.h"

typedef enum _MP5K_RET_CODE {
	MP5K_OK ,
	MP5K_CLIENT_FAILED,
	MP5K_DISCONNECT ,

	MP5K_SYNC_TIMEOUT ,

	MP5K_MEAS_FAILED ,
	MP5K_MEAS_TIME_OUT ,

	MP5K_HANDLE_ERROR ,
	MP5K_CHECK_ID_TIMEOUT ,
	MP5K_CHECK_ID_TIMEOUT_BY_CUSTOM ,
	MP5K_CHECK_ID_INCONSISTENT ,
	MP5K_WAIT_ID_FAILED ,
	MP5K_CONNECT_TIMEOUT ,

	MP5K_STANDARD_ERROR ,
	MP5K_USEPORT_ERROR ,
	MP5K_GET_PORT_ERROR,

	MP5K_TESTER_NULL ,
	
	// TX
	MP5K_TX_FREQUENCY_ERROR ,
	MP5K_TX_REF_LEVEL_ERROR ,
	MP5K_TX_CAPTURE_LENGTH_ERROR ,
	MP5K_TX_READ_PHASE_ERROR ,
	MP5K_TX_READ_FREQUENCY_ERROR ,
	MP5K_TX_READ_SYMBOL_CLOCK_ERROR ,
	MP5K_TX_READ_LO_LEAKAGE_ERROR ,
	MP5K_TX_READ_MASK_ERROR ,
	MP5K_TX_READ_EVM_ERROR ,
	MP5K_TX_READ_MASK_ERROR_ERROR ,
	MP5K_TX_SET_WIDE_ERROR ,
	
	// RX
	MP5K_RX_FREQUENCY_ERROR ,
	MP5K_RX_OUTPUT_POWER_ERROR ,
	MP5K_RX_PACKET_COUNT_ERROR ,
	MP5K_RX_WAVEFORM_ERROR ,
	MP5K_RX_START_TIMEOUT ,
	MP5K_RX_GAP_ERROR ,
	MP5K_RX_STOP_ERROR ,

	MP5K_RX_SET_BANDWIDTH_ERROR ,
	MP5K_RX_SET_RATE_ERROR ,

	// Trigger
	MP5K_TRIGGER_TYPE_ERROR ,
	MP5K_TRIGGER_THRESHOLD_ERROR ,
	MP5K_TRIGGER_OFFSET_ERROR ,
	MP5K_TRIGGER_GAPTIME_ERROR ,

	// Correction
	MP5K_CORR_FREQUENCY_ERROR ,
	MP5K_CORR_CHANNEL_ESTIMATION_ERROR ,

	// Trim
	MP5K_TRIM_FREQUENCY_ERROR ,
	MP5K_TRIM_RBW_ERROR ,
	MP5K_TRIM_REF_LEVEL_ERROR ,
	MP5K_TRIM_POWER_TOO_LOW_ERROR ,
	MP5K_TRIM_GETOFFSET_TIMEOUT ,

	// Calibration
	MP5K_CAL_FREQUENCY_ERROR ,
	MP5K_CAL_REF_LEVEL_ERROR ,

	//
	MP5K_REGISTER_ID_TIMEOUT ,
	MP5K_REMOVE_ID_TIMEOUT ,
	
	MP5K_TXEX_READ_MARGIN_ERROR ,
	MP5K_TXEX_READ_FREQ_ERROR ,
	MP5K_TXEX_READ_FLATNESS_ERROR ,
	MP5K_TXEX_READ_RAMP_ON_ERROR ,
	MP5K_TXEX_READ_RAMP_OFF_ERROR ,

	MP5K_SET_SAMPLING_RATE_ERROR,

	//BT
	MP5K_AGC_ERROR,


	//GPS
	MP5K_GPS_POWER_ERROR,
	MP5K_GPS_DOPPLER_ERROR,
	MP5K_GPS_SVID_ERROR,
	MP5K_GPS_PATTERN_ERROR,
	MP5K_GPS_RF_ON_ERROR,
	MP5K_GPS_RF_OFF_ERROR,
	MP5K_GPS_CW_SIGNAL_ERROR,
	MP5K_GPS_PATH_ERROR,

} MP5K_RET_CODE ;

std::string MessageTable[] = {
	"MP5K Sys:OK" ,
	"MP5k Sys:Clinet create failed" ,
	"MP5K Sys:Connect timeout" ,
	"MP5K Sys:Sync timeout" ,
	"MP5K Sys:Measure failed" ,
	"MP5K Sys:Measure timeout" ,
	"MP5K Sys:Handle error" ,
	"MP5K Sys:Check ID timeout" ,
	"MP5K Sys:Check ID timeout by custom" ,
	"MP5K Sys:Check ID inconsistent" ,
	"MP5K Sys:Wait ID failed" ,
	"MP5K Sys:Connect timeout" ,
	"MP5K Sys:Standard error" ,
	"MP5K Sys:Use port error" ,
	"MP5K Sys:Get port error" ,
	"MP5K Tester Object NULL" ,
	"MP5K TX:Frequency error" ,
	"MP5K TX:Reference level error" ,
	"MP5K TX:Capture length error" ,
	"MP5K TX:Read Phase error failed" ,
	"MP5K TX:Read Frequency error failed" , 
	"MP5K TX:Read Symbol clock error failed" ,
	"MP5K TX:Read Lo Leakage error failed" ,
	"MP5K TX:Read MASK error failed" ,
	"MP5K TX:Read EVM error failed" ,
	"MP5K TX:Read Mask Error error failed" ,
	"MP5K TX:Set Wide Band error failed" ,
	"MP5K RX:Frequency error" ,
	"MP5K RX:Output power error" ,
	"MP5K RX:Packet count error" ,
	"MP5K RX:Waveform error" ,
	"MP5K RX:Start timeout" ,
	"MP5K RX:GAP error" ,
	"MP5K RX:Strop error" ,
	"MP5K RX:Set Bandwidth error" ,
	"MP5K RX:Set Rate error" ,
	"MP5K Trigger:Type error" ,
	"MP5K Trigger:Threshold error" ,
	"MP5K Trigger:Offset error" ,
	"MP5K Trigger:Gap Time error" ,
	"MP5K Correction:Frequency Type error" ,
	"MP5K Correction:Cahannel Estimation Type error" ,
	"MP5K Trim:Freuqency error" ,
	"MP5K Trim:RBW error" ,
	"MP5K Trim:Reference Level error" ,
	"MP5K Trim:Power low < -30dBm" ,
	"MP5K Trim:Get Offset timeout" ,
	"MP5K Cal:Frequency error" ,
	"MP5K Cal:Reference Level error" ,
	"MP5K Sys:Register Id Timeout" ,
	"MP5K Sys:Remove Id Timeout" ,

	"MP5K TXEx:Read Margin error failed" ,
	"MP5K TXEx:Read Freq error failed" ,
	"MP5K TXEx:Read Flatness error failed" ,
	"MP5K TXEx:Read Ramp on error failed" ,
	"MP5K TXEx:Read Ramp off error failed" ,

	"MP5K Sys:Setting Sampling Rate failed",
	"MP5K Sys::Setting AGC failed"


	//GPS
	"MP5K GPS::Set Power failed",
	"MP5K GPS::Set Doppler failed",
	"MP5K GPS::Set SVID failed",
	"MP5K GPS::Set Pattern failed",
	"MP5K GPS::Set RF ON failed",
	"MP5K GPS::Set RF OFF failed",
	"MP5K GPS::Set CW Signal failed",
	"MP5K GPS::Set Path failed",
};

#endif