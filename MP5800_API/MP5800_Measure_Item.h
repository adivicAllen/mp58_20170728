#ifndef  MP5800_MEASURE_ITEM_H
#define  MP5800_MEASURE_ITEM_H
 
enum MEASURE_ITEM {
	CHANNEL_POWER_dBm,
	PEAK_POWER_dBm,
	PEAK_FREQUENCY_MHz,

	FSK_FREQUENCY_DEVIATION_kHz,
	FSK_FREQUENCY_OFFSET_kHz,
	FSK_FREQUENCY_DRIFT_kHz,
	FSK_DEVIATION_ERROR_kHz,
	FSK_FSK_ERROR_kHz,

	WIFI_Power_dBm,         
	WIFI_EVM_dB,
	WIFI_Spectrum_Mask_bool,    
	WIFI_Phase_Error_deg,  
	WIFI_Freq_Error_kHz,   
	WIFI_SymClk_Error_ppm,
	WIFI_Lo_Leakage_dB,

    BT_Power_dBm,         
 	BT_BDR_Initial_Freq_Error_kHz,
	BT_BDR_Bandwidth_20dB_MHz,
	BT_BDR_FreqDrift_kHz,  
    BT_BDR_DeltaF1Avg_kHz, 
	BT_BDR_DeltaF2Avg_kHz,     
	BT_BDR_DeltaF2Max_kHz,	    
	BT_EDR_DEVM_percent,       
	BT_EDR_DEVM_Pk_percent,        
	BT_LE_DeltaF1Avg_kHz,   
	BT_LE_DeltaF2Avg_kHz,      
	BT_LE_DeltaF2Max_kHz,     
};

#endif