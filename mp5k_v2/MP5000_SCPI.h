/*------------------------------------------*/
/*			ADIVIC Tecknology Co.		    */
/*	ADIVIC Wireless Test Station MP5000		*/
/*			MP5000 SCPI		V0.0.032		*/
/*											*/
/*				2016/10/18					*/
/*------------------------------------------*/


#ifndef MP5000_SCPI_H


#define SET_OFDM_MODE								"WIFI:OFDM"
#define SET_DSSS_MODE								"WIFI:DSSS"
#define SET_GPRF_MODE								"GPRF"
#define SET_BT_MODE									"BT"
#define QUERY_MODE									"MODule?"
#define SET_DATA_TO_FILE							"MMEMory:DATA"
#define QUERY_DATA_FROM_FILE						"MMEMory:DATA? <FILE_NAME>"
#define DELETE_FILE									"MMEMory:DELete <FILE_NAME>" 
#define CREATE_FOLDER								"MMEMory:MDirectory <FILE_NAME>"

// VSA Control
#define	VSA_INPUT_ON								"INPut:STARt"
#define	VSA_INPUT_OFF								"INPut:STOP"
#define	VSA_SET_FREQ								"INPut:FREQuency"
#define	VSA_QUERY_FREQ								"INPut:FREQuency?"
#define	VSA_SET_SAMPLING_RATE						"INPut:SRATe"
#define	VSA_QUERY_SAMPLING_RATE						"INPut:SRATe?"
#define	VSA_SET_REF_LEVEL							"INPut:LEVel:REFerence"
#define	VSA_QUERY_REF_LEVEL							"INPut:LEVel:REFerence?"
#define	VSA_AUTO_GAIN								"INPut:LEVel:REFerence:AUTO"
#define	VSA_SET_CAPTURE_LENGTH						"INPut:CAPTure:TIME"
#define	VSA_QUERY_CAPTURE_LENGTH					"INPut:CAPTure:TIME?"
#define	VSA_SAVE_WAVEFORM							"CAPTure:STORe"
#define	VSA_LOAD_WAVEFORM							"INPut:WAVeform:LOAD"



// VSG Control
#define VSA_AVERAGE_DATA							"AVERage?"
#define VSA_MAXIMUM_DATA							"MAXimum?"
#define VSA_MINIMUM_DATA							"MINimum?"

#define START_VSG_SYNC								"OUTPut:STARt"								
#define START_VSG_ASYNC								"OUTPut:STARt:ASYNchronous"
#define STOP_VSG									"OUTPut:STOP"
#define VSG_FREQUENCY								"OUTPut:FREQuency"
//#define "OUTPut:FREQuency?"
#define VSG_OUTPUT_POWER							"OUTPut:POWer:LEVel"
//#define "OUTPut:POWer:LEVel?"
#define VSG_LOAD_FILE								"OUTPut:WAVeform:LOAD <FILE_NAME>"
#define VSG_OUTPUT_COUNT							"OUTPut:LOOP:COUNt"
//#define "OUTPut:LOOP:COUNt?"
//#define "OUTPut:LOOP:COUNt:MAXimum?"
//#define "OUTPut:MODulation:STATe < ON | OFF >"
//#define "OUTPut:MODulation:STATe?"
//#define "OUTPut:POWer:STATe < ON | OFF >"
//#define "OUTPut:POWer:STATe?"
//#define "OUTPut:WAVeform:CWAV:OFFSet"
//#define "OUTPut:WAVeform:CWAV:OFFSet?"
//#define "OUTPut:ATTenuation:PORT{1:4}?"

//----------VSA Measure----------//

#define VSA_CHANNEL_POWER						"FETCh:SEGMent1:POWer?"
//#define "FETCh:SEGMent1:POWer:AVERage?
//#define "FETCh:SEGMent1:POWer:MAXimum?
//#define "FETCh:SEGMent1:POWer:MINimum?
#define VSA_PEAK_POWER							"FETCh:SEGMent1:POWer:PEAK?"
//#define "FETCh:SEGMent1:POWer:PEAK:AVERage?
//#define "FETCh:SEGMent1:POWer:PEAK:MAXimum?
//#define "FETCh:SEGMent1:POWer:PEAK:MINimum?
		
//#define "FETCh:SEGMent1:OFDM:ALL?"
//#define "FETCh:SEGMent1:OFDM:ALL:AVERage?
//#define "FETCh:SEGMent1:OFDM:ALL:MAXimum?
//#define "FETCh:SEGMent1:OFDM:ALL:MINimum?
#define VSA_OFDM_POWER							"FETCh:SEGMent1:OFDM:POWer?"
//#define "FETCh:SEGMent1:OFDM:POWer:AVERage?
//#define "FETCh:SEGMent1:OFDM:POWer:MAXimum?
//#define "FETCh:SEGMent1:OFDM:POWer:MINimum?
#define VSA_OFDM_PREAMBLE_POWER					"FETCh:SEGMent1:OFDM:POWer:PREamble?"
//#define "FETCh:SEGMent1:OFDM:POWer:PREamble:AVERage?
//#define "FETCh:SEGMent1:OFDM:POWer:PREamble:MAXimum?
//#define "FETCh:SEGMent1:OFDM:POWer:PREamble:MINimum?
#define VSA_OFDM_PREAMBLE_DETAIL_POWER			"FETCh:SEGMent1:OFDM:POWer:DETail?"
//#define "FETCh:SEGMent1:OFDM:POWer:DETail:AVERage?
//#define "FETCh:SEGMent1:OFDM:POWer:DETail:MAXimum?
//#define "FETCh:SEGMent1:OFDM:POWer:DETail:MINimum?
#define VSA_OFDM_EVM							"FETCh:SEGMent1:OFDM:EVM?"
//#define "FETCh:SEGMent1:OFDM:EVM:AVERage?
//#define "FETCh:SEGMent1:OFDM:EVM:MAXimum?
//#define "FETCh:SEGMent1:OFDM:EVM:MINimum?
#define VSA_OFDM_EVM_DATA						"FETCh:SEGMent1:OFDM:EVM:DATA?"
//#define "FETCh:SEGMent1:OFDM:EVM:DATA:AVERage?
//#define "FETCh:SEGMent1:OFDM:EVM:DATA:MAXimum?
//#define "FETCh:SEGMent1:OFDM:EVM:DATA:MINimum?
#define VSA_OFDM_EVM_PILOT						"FETCh:SEGMent1:OFDM:EVM:PILot?"
//#define "FETCh:SEGMent1:OFDM:EVM:PILot:AVERage?
//#define "FETCh:SEGMent1:OFDM:EVM:PILot:MAXimum?
//#define "FETCh:SEGMent1:OFDM:EVM:PILot:MINimum?
#define VSA_OFDM_EVM_PERCENTAGE					"FETCh:SEGMent1:OFDM:EVM:PERcentage?"
//#define "FETCh:SEGMent1:OFDM:EVM:PERcentage:AVERage?
//#define "FETCh:SEGMent1:OFDM:EVM:PERcentage:MAXimum?
//#define "FETCh:SEGMent1:OFDM:EVM:PERcentage:MINimum?
#define VSA_OFDM_PHASE_ERROR					"FETCh:SEGMent1:OFDM:PERRor?"
//#define "FETCh:SEGMent1:OFDM:PERRor:AVERage?
//#define "FETCh:SEGMent1:OFDM:PERRor:MAXimum?
//#define "FETCh:SEGMent1:OFDM:PERRor:MINimum?
#define VSA_OFDM_FREQUENCY_ERROR				"FETCh:SEGMent1:OFDM:FERRor?"
//#define "FETCh:SEGMent1:OFDM:FERRor:AVERage?
//#define "FETCh:SEGMent1:OFDM:FERRor:MAXimum?
//#define "FETCh:SEGMent1:OFDM:FERRor:MINimum?
#define VSA_OFDM_SYMBLE_CLOCK_ERROR				"FETCh:SEGMent1:OFDM:SCERror?"
//#define "FETCh:SEGMent1:OFDM:SCERror:AVERage?
//#define "FETCh:SEGMent1:OFDM:SCERror:MAXimum?
//#define "FETCh:SEGMent1:OFDM:SCERror:MINimum?
#define VSA_OFDM_LO_LEAKAGE						"FETCh:SEGMent1:OFDM:LOLeakage?"
//#define "FETCh:SEGMent1:OFDM:LOLeakage:AVERage?
//#define "FETCh:SEGMent1:OFDM:LOLeakage:MAXimum?
//#define "FETCh:SEGMent1:OFDM:LOLeakage:MINimum?

#define VSA_OFDM_OCCUPIED_Bandwidth				"FETCh:SEGMent1:OFDM:SPECtrum:OBW?"
#define VSA_OFDM_BEAMFORMED						"FETCh:SEGMent1:OFDM:BEAMformed?"

#endif