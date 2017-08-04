#ifndef  MP5800_API_H
#define  MP5800_API_H
 #include <QtCore/qglobal.h>

#ifdef MP5800_API_EXPORTS
    #define MP5800_API extern "C"  Q_DECL_EXPORT
 #else
    #define MP5800_API extern "C"   Q_DECL_IMPORT
 #endif


/* ============================================================================
**	ADIVIC
**  expolre MP5800 API name
** ========================================================================== */
  
#include "MP5800_API_define.h" 
using namespace MP5800;
/* ---------------------------------------------------------------------------
**	MP5800 Connection
** ------------------------------------------------------------------------- */

MP5800_API  UINT  MP5800_connect( const char *IniFilePath , const bool bDisplay = false , const bool bDebugMode = false);
MP5800_API  UINT  MP5800_disconnect( void );
 
/* ---------------------------------------------------------------------------
**	MP5800 config
** ------------------------------------------------------------------------- */
MP5800_API UINT  MP5800_setConfig( const RF_PORT rfPort ,	const MP5800_CONFIG& );

/* ---------------------------------------------------------------------------
**	MP5800 create table, add item to table, get table
** ------------------------------------------------------------------------- */
MP5800_API UINT  MP5800_createTable( int tableIndex  );
MP5800_API UINT  MP5800_addMeasureItemToTable( int tableIndex , MEASURE_ITEM measureItem );
MP5800_API UINT  MP5800_getTable( const RF_PORT rfPort, int tableIndex );
MP5800_API UINT  MP5800_getMeasureItemFromTable( int tableIndex , MEASURE_ITEM measureItem , double &measureValue );

/* ---------------------------------------------------------------------------
**	MP5800 VSG/VSA function
** ------------------------------------------------------------------------- */
MP5800_API UINT  MP5800_setVSG( const RF_PORT rfPort ,	const MP5800_VSG_CONFIG& );
MP5800_API UINT  MP5800_setVSG_Run( const RF_PORT , bool vsgOnOff = true );
MP5800_API UINT  MP5800_setVSA( const RF_PORT ,  const MP5800_VSA_CONFIG&   );
MP5800_API UINT  MP5800_getVSA_Value( const RF_PORT rfPort, MP5800_VSA_MEASURE&);
 
/* ---------------------------------------------------------------------------
**	MP5800 debug function
** ------------------------------------------------------------------------- */

MP5800_API UINT  MP5800_saveVSA_WaveformToFile( const RF_PORT, const char *waveformFileName);

/* ---------------------------------------------------------------------------
**	MP5800 FSK modulation
** ------------------------------------------------------------------------- */

MP5800_API UINT  MP5800_setFSK_VSA( const RF_PORT, const MP5800_FSK_CONFIG&,  const MP5800_VSA_CONFIG&);
MP5800_API UINT  MP5800_getFSK_VSA_Value( const RF_PORT, MP5800_FSK_MEASURE& );


/* ---------------------------------------------------------------------------
**	MP5800 wifi modulation
** ------------------------------------------------------------------------- */

MP5800_API UINT  MP5800_setWIFI_VSG( const RF_PORT, const MP5800_WIFI_CONFIG& , const MP5800_VSG_CONFIG&);
MP5800_API UINT  MP5800_setWIFI_VSG_Run(  const RF_PORT , bool vsgOnOff = true );
 
MP5800_API UINT  MP5800_setWIFI_VSA( const RF_PORT, const MP5800_WIFI_CONFIG& , const MP5800_VSA_CONFIG&);
MP5800_API UINT  MP5800_getWIFI_VSA_Value(const RF_PORT , MP5800_WIFI_MEASURE&);

/* ---------------------------------------------------------------------------
**	MP5800 BT modulation
** ------------------------------------------------------------------------- */
 
MP5800_API UINT  MP5800_setBT_VSG( const RF_PORT, const MP5800_BT_CONFIG& , const MP5800_VSG_CONFIG& );
MP5800_API UINT  MP5800_setBT_VSG_Run(  const RF_PORT , const bool OnOff = true );
 
MP5800_API UINT  MP5800_setBT_VSA( const RF_PORT, const MP5800_BT_CONFIG& ,const MP5800_VSA_CONFIG&);
MP5800_API UINT  MP5800_getBT_VSA_Value(const RF_PORT , MP5800_BT_MEASURE&);
 

#endif
