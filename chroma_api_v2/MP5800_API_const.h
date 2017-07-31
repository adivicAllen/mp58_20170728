#ifndef  MP5800_API_CONST_H
#define  MP5800_API_CONST_H

#define MP5800_SUCCESS    0
#define MP5800_ERROR      100
#define MP5800_ERROR_VALUE   -99999.


/* ---------------------------------------------------------------------------
**	MP5800 VSA ERROR
** ------------------------------------------------------------------------- */
#define MP5800_VSA_ERROR 200
#define MP5800_SET_SAMPLE_RATE_ERROR           ( MP5800_VSA_ERROR + 1 )
#define MP5800_SET_TRIGGER_TYPE_ERROR          ( MP5800_VSA_ERROR + 2 )
#define MP5800_GET_PEAK_POWER_ERROR            ( MP5800_VSA_ERROR + 3 )
#define MP5800_GET_CHANNEK_POWER_ERROR         ( MP5800_VSA_ERROR + 4 )
#define MP5800_SET_RBW_ERROR                   ( MP5800_VSA_ERROR + 5 )  
#define MP5800_SET_REFERENCE_LEVEL_FAIL_ERROR  ( MP5800_VSA_ERROR + 6 )  
#define MP5800_SET_SAMPLING_RATE_FAIL_ERROR    ( MP5800_VSA_ERROR + 7 ) 
#define MP5800_SET_WIFI_RX_ERROR               ( MP5800_VSA_ERROR + 8 ) 
#define MP5800_SET_WIFI_RX_ADVANCE_ERROR        ( MP5800_VSA_ERROR + 9 ) 
#define MP5800_GET_WIFI_RX_ERROR                ( MP5800_VSA_ERROR +10 ) 
#define MP5800_GET_BT_RX_VALUE_ERROR            ( MP5800_VSA_ERROR +11 )
#define MP5800_SET_BT_RX_ERROR                   ( MP5800_VSA_ERROR + 12)
#define MP5800_SET_CAPTURE_LENGTH_ERROR         ( MP5800_VSA_ERROR + 13)
#define MP5800_SET_GAP_TIME_ERROR               ( MP5800_VSA_ERROR + 14) 
#define MP5800_SET_TRIGGER_TIME_OUT_ERROR       ( MP5800_VSA_ERROR + 15) 
#define MP5800_SAVE_WAVEFORM_TO_FILE_ERROR       ( MP5800_VSA_ERROR + 16) 
#define MP5800_SET_TRIGGER_OFFSET_ERROR          ( MP5800_VSA_ERROR + 17)


/* ---------------------------------------------------------------------------
**	MP5800 VSG ERROR
** ------------------------------------------------------------------------- */
#define MP5800_VSG_ERROR 250
#define MP5800_SET_FREQUENCY_ERROR             ( MP5800_VSG_ERROR + 1 )
#define MP5800_SET_OUTPUT_POWER_ERROR          ( MP5800_VSG_ERROR + 2)

#define MP5800_INITIAL_FAIL                    ( MP5800_VSG_ERROR + 3 )
#define MP5800_DISCONNECT_ERROR                ( MP5800_VSG_ERROR + 4 )
#define MP5800_SET_CW_FAIL_ERROR               ( MP5800_VSG_ERROR + 5 )
#define MP5800_SET_RF_ON_OFF_FAIL_ERROR        ( MP5800_VSG_ERROR + 6)

#define MP5800_SET_CENTER_FREQUENCY_ERROR       ( MP5800_VSG_ERROR + 7 )

#define MP5800_SET_TX_POWER_ERROR               ( MP5800_VSG_ERROR + 8 )
#define MP5800_SET_MODULATION_FAIL_ERROR        ( MP5800_VSG_ERROR + 9 )

#define MP5800_SET_WIFI_TX_ERROR                 ( MP5800_VSG_ERROR + 10)

#define MP5800_SET_BT_TX_ERROR                    ( MP5800_VSG_ERROR + 11)
#define MP5800_SET_WIFI_TX_RUN_ERROR              ( MP5800_VSG_ERROR + 12)
#define MP5800_SET_BT_TX_RUN_ERROR                ( MP5800_VSG_ERROR + 13)
#define MP5800_SET_CW_FREQUENCY_OFFSET_ERROR      ( MP5800_VSG_ERROR + 14 )
#define MP5800_LOAD_WAVEFORM_ERROR                 ( MP5800_VSG_ERROR + 15 )
#define MP5800_SET_PALYBACK_GAP_TIME_ERROR         ( MP5800_VSG_ERROR + 16)
/* ---------------------------------------------------------------------------
**	MP5800 CONFIG ERROR
** ------------------------------------------------------------------------- */
#define MP5800_CONFIG_ERROR 300
#define MP5800_Set_4Port_Switch_FAIL_ERROR     ( MP5800_CONFIG_ERROR + 1 )
#define MP5800_SET_CABLE_LOSS_ON_OFF_ERROR     ( MP5800_CONFIG_ERROR + 2)
#define MP5800_SET_SPECTRUM_ERROR              ( MP5800_CONFIG_ERROR + 3 )
#define  MP5800_SET_DISPLAY_ON_OFF_FAIL        ( MP5800_CONFIG_ERROR + 4 )
#define MP5800_SET_DISTRIBUTION_MODE_ERROR     ( MP5800_CONFIG_ERROR + 5 )
#define MP5800_SET_ENABLE_CABLE_LOSS_ERROR     ( MP5800_CONFIG_ERROR + 6 )
#define MP5800_SET_STANDARD_ERROR              ( MP5800_CONFIG_ERROR + 7 )
#define MP5800_SET_INITAL_PARAMETER_FAIL       ( MP5800_CONFIG_ERROR + 8 )
#define MP5800_SET_WIFI_OFDM_ERROR             ( MP5800_CONFIG_ERROR + 9 ) 
#define MP5800_SET_WIFI_DSSS_ERROR             ( MP5800_CONFIG_ERROR + 10 )
/* -------------------------------------------------------------------------------
** Modualation  error 
** ------------------------------------------------------------------------------ */
#define MP5800_MEASURE_ERROR 350
#define MP5800_GET_FSK_MEASURE_ERROR           ( MP5800_MEASURE_ERROR + 1 ) 
#define MP5800_SET_DEMODULATION_ERROR          ( MP5800_MEASURE_ERROR + 2 ) 
#define MP5800_SET_FSK_VSA_ERROR               ( MP5800_MEASURE_ERROR + 3 )
#define MP5800_SET_VSA_PORT_ERROR              ( MP5800_MEASURE_ERROR + 4 )
#define MP5800_GET_ALL_MEASUREMENT_ERROR       ( MP5800_MEASURE_ERROR + 5)
#define MP5800_GET_FSK_FREQDEVIATION_ERROR     ( MP5800_MEASURE_ERROR + 6)
#define MP5800_SET_FSK_DEMODULATION_ERROR      ( MP5800_MEASURE_ERROR + 7)
#define MP5800_GET_PEAK_POWER_FREQUENCY_ERROR  ( MP5800_MEASURE_ERROR + 8)
#define MP5800_GET_WIFI_MEASUREMENT_ERROR      ( MP5800_MEASURE_ERROR + 9)

/* -------------------------------------------------------------------------------
** Modualation  error 
** ------------------------------------------------------------------------------ */
#define MP5800_TABLE_ERROR 400
#define MP5800_CREATE_CUSTOMIZED_TABLE_ERROR    ( MP5800_TABLE_ERROR + 1 ) 
#define MP5800_ADD_MEASURE_ITEM_ERROR           ( MP5800_TABLE_ERROR + 2 ) 

#endif 