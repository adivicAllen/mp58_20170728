#ifndef  MP5800_API_CONST_H
#define  MP5800_API_CONST_H

#define MP5800_SUCCESS    0
#define MP5800_ERROR      100
#define MP5800_ERROR_VALUE   -99999.


/* ---------------------------------------------------------------------------
**	MP5800 VSA ERROR
** ------------------------------------------------------------------------- */
#define MP5800_VSA_ERROR 200
#define MP5800_GET_PEAK_POWER_ERROR            ( MP5800_VSA_ERROR + 1 )
#define MP5800_GET_CHANNEK_POWER_ERROR         ( MP5800_VSA_ERROR + 2 )

/* ---------------------------------------------------------------------------
**	MP5800 VSG ERROR
** ------------------------------------------------------------------------- */
#define MP5800_VSG_ERROR 220
#define MP5800_SET_FREQUENCY_ERROR             ( MP5800_VSG_ERROR + 1 )
#define MP5800_SET_SAMPLE_RATE_ERROR           ( MP5800_VSG_ERROR + 2 )
#define MP5800_SET_TRIGGER_TYPE_ERROR          ( MP5800_VSG_ERROR + 3 )
#define MP5800_SET_OUTPUT_POWER_ERROR          ( MP5800_VSG_ERROR + 4 )

#define MP5800_INITIAL_FAIL                    ( MP5800_VSG_ERROR + 5 )
#define MP5800_DISCONNECT_ERROR                ( MP5800_VSG_ERROR + 6 )

#define MP5800_SET_SPECTRUM_ERROR                ( MP5800_VSG_ERROR + 7 )
#define MP5800_SET_CENTER_FREQUENCY_ERROR         ( MP5800_VSG_ERROR + 8 )

#define MP5800_SET_TX_POWER_ERROR            ( MP5800_VSG_ERROR + 9 )
 
 
#endif