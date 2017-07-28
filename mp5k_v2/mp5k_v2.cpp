// mp5k_v2.cpp : Defines the exported functions for the DLL application.
//
#ifdef VC
  #include "stdafx.h"
#endif

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QString>
#include <vector>
#include <set>
#include <string>
#include <fstream>
using namespace std;
#include <boost/timer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/hex.hpp>
#include <bitset>
 
#include "mp5k_v2.h"
#include "MP5000_SCPI.h"

#include "ADIVIC_Sync_TelnetClient.h"
#include "AdivicGlobal.hpp"
#include "adivic_thread.hpp"
#include "mp5k_error.h"
#include <unistd.h>




#define sprintf_s sprintf   // allenm  201707234
#define strcpy_s  strcpy

void Sleep(int sleepTime)
{
    usleep(((useconds_t)sleepTime)*1000);
}

class mp5k_log {
    private:
        std::ofstream pLog;
		char buff[1024];
    public:
        inline mp5k_log(void)
        {
            // create the file
            WriteOpen();
        };
        inline ~mp5k_log(void)
        {
            // close the file
            WriteClose();
        };
        inline bool WriteOpen()
        {
			pLog.open("mp5k_log.txt", std::ofstream::out);
			return pLog.is_open();
        };
        inline bool WriteClose()
        {
            pLog.close();
            return true;
        };
        inline bool WriteLine(const char* message)
        {
          //  SYSTEMTIME st, lt;
          //  GetSystemTime(&st);
           // GetLocalTime(&lt);
            QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间

            QString  timeStamp = time.toString("yyyy-MM-dd hh:mm:ss:zzz"); //设置显示格式
            /*
            sprintf (buff , "[%02d:%02d:%02d:%03d]:[%s]\n" ,
                     lt.wHour ,
                     lt.wMinute ,
                     lt.wSecond ,
                     lt.wMilliseconds ,
                     message
                     );
            */
            pLog <<  timeStamp.toStdString();
			pLog << buff;
			pLog.flush();
            return true;
        };
};

#define BUFFER_LENGTH 513

// Buffer

char	strBuffer[1024]; // R/W
std::string ackBuffer;
bool	isFourPort = false;
bool	isWifi_TX_Sync = false;
bool	isBT_TX_Sync = false;
UINT	WIFI_RX_ASYNC_MODE = 0;
UINT	gloMyID = 0;
UINT	gloNextID = 0;
UINT	USER_ID_HANDLE = 0;

std::set<std::string> WaveformQueue;

const UINT	rx_time_out = 12000; // RX time out
const UINT	BT_rx_time_out = 20000; // RX time out


#if MP5KLOG
mp5k_log Mp5kLog;
#endif

class AdivicTester
{


    public:
        bool MultiDUTEnable;

        AdivicTester( void )
        {

        }

        AdivicTester( const AdivicTester& ) {}
        ~AdivicTester(void ) {}

        // Current Setting
        MP5K_XtalTrimConfig			gloXtalTrimConfig;
        MP5K_CalConfig				gloCalConfig;
        MP5K_TxTestConfig			gloTXConfig;
        MP5K_RxTestConfig			gloRXConfig;
        MP5K_TriggerConfig			gloTrigger;
        MP5K_CorrectionConfig		gloCorrection;


		//BlueTooth
		MP5K_BT_TxTestConfig		gloBtTXConfig;
		MP5K_BT_RxTestConfig		gloBtRXConfig;

		vector<ADIVIC_Sync_TelnetClient*> mp5k_list;


        UINT Handle;


};

AdivicTester* Tester = nullptr;


bool isInit = false;
bool isAdd = false;
char preIP[16] = {"\0"}; 
		
/*GPS Member*/
AdivicTester* GPSTester = nullptr;
bool isGPSInit = false;
bool isGPSAdd = false;
char preGPSIP[16] = {"\0"};

std::string trigger_set[] =
{
    "IMMediate" , "POSTive" , "NEGative" , "LEVel"
};

std::string freq_est[] =
{
    "OFF", "STF", "LTF"
};
	
std::string data_est[] =
{
    "OFF", "LTF", "DATA"
};

const static bool SubString( std::string& data, int beginPos)
{
	if( data.length() > beginPos )
	{
		data = data.substr( 2, data.size() );
		return true;
	}
    return false;
}


void ResetSub()
{
		isFourPort = false;
		isWifi_TX_Sync = false;
		isBT_TX_Sync = false;
		WIFI_RX_ASYNC_MODE = 0;
		gloMyID = 0;
		gloNextID = 0;
		USER_ID_HANDLE = 0;

		isInit = false;
		isAdd = false;
		 isGPSInit = false;
		isGPSAdd = false;


		strBuffer[1024]; // R/W
		memset(strBuffer,0,sizeof(strBuffer));
		memset(preIP,0x20,sizeof(preIP));
		memset(preGPSIP,0x20,sizeof(preGPSIP));
        ackBuffer = "";
 
 

       WaveformQueue.clear();

 
	   Tester = nullptr	;
 
       GPSTester = nullptr;
 
 
		 
}

UINT mp5k_init( void )
{
	if( !isInit )
	{
        Tester = new  AdivicTester();  // strang , //allen 20170723
    //     Tester = new AdivicTask< AdivicTester >();
        if ( !Tester )
		{
			// write to log
#if MP5KLOG
			sprintf_s( strBuffer , "mp5k_init:[Init failed]" );
			Mp5kLog.WriteLine( strBuffer );
#endif
			return MP5K_TESTER_NULL;
		}
#ifdef OLD_CLIENT
        AdivicTClient* tmp = nullptr;	
#else
		ADIVIC_Sync_TelnetClient* tmp = nullptr;		
#endif
        Tester->mp5k_list.push_back( tmp ); // no use
		// default multi-dut variable
        Tester->MultiDUTEnable = false;
		// write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_init:[MultiDUT = %d]", Tester->MultiDUTEnable );
		Mp5kLog.WriteLine( strBuffer );
#endif
		gloMyID = 1;
		gloNextID = 1;
		// write to log
#if MP5KLOG
		sprintf_s( strBuffer , "mp5k_init:[MyID = %d,NextID = %d]", gloMyID , gloNextID );
		Mp5kLog.WriteLine( strBuffer );
#endif
    //if ( !Tester->create( NORMAL , 1024 ) )
   // if ( !Tester->create( QThread::NormalPriority , 1024 ) )
      if ( !Tester )

		{
			return MP5K_TESTER_NULL;
		}
		isInit = true;
		// write to log
#if MP5KLOG
		sprintf_s( strBuffer , "mp5k_init:[Watch dog is standby]" );
		Mp5kLog.WriteLine( strBuffer );
#endif
	}
    return MP5K_OK;
}

UINT mp5k_add( const char* ip, UINT& handle )
{
	ADIVIC_Sync_TelnetClient* tmp = new ADIVIC_Sync_TelnetClient( "MP5000_CMD" );

	if ( tmp == nullptr )
	{
		// write to log
#if MP5KLOG
		sprintf_s( strBuffer , "mp5k_add:[Client create failed]" );
		Mp5kLog.WriteLine( strBuffer );
#endif
		return MP5K_CLIENT_FAILED;
	}

	if( 0 == strcmp(preIP,ip) )
	{
        if( Tester->mp5k_list.at( handle )->IsConnected() )
		{
			return ::MP5K_OK;
		}
        Tester->mp5k_list.clear();
		ADIVIC_Sync_TelnetClient* temp = nullptr;
        Tester->mp5k_list.push_back( temp ); // no use
	}
	if ( !tmp->Open( ip , 5499 , 2 ) )    // 2s, timeout, disconnect
	{
		tmp->Close();
		delete tmp;
// write to log
#if MP5KLOG
		sprintf_s( strBuffer , "mp5k_add:[Client connect timeout]" );
		Mp5kLog.WriteLine( strBuffer );
#endif
		return MP5K_CONNECT_TIMEOUT; // id
	}
	else
	{
        int id = Tester->mp5k_list.size();
        Tester->mp5k_list.push_back( tmp );
		handle = id;
        strcpy_s(preIP,ip);

        Tester->mp5k_list.at( handle )->Send( VSA_INPUT_ON );
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
// write to log
#if MP5KLOG
			sprintf_s(strBuffer, "mp5k_add:[MP5000 Start]", ip, id);
			Mp5kLog.WriteLine( strBuffer );
#endif
			return MP5K_CONNECT_TIMEOUT;
		}
		/*
        Tester->mp5k_list.at( handle )->Send( "INPut:TXQuality:SKIP:COUNt 5" );
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
// write to log
#if MP5KLOG
			sprintf_s(strBuffer, "mp5k_add:[MP5000 Start]", ip, id);
			Mp5kLog.WriteLine( strBuffer );
#endif
			return MP5K_CONNECT_TIMEOUT;
		}
		*/
#if MP5000API
  // allen 20170605
		ADIVIC_Sync_TelnetClient* ID_TEMP = new ADIVIC_Sync_TelnetClient( "" );

		if( ID_TEMP->Open( ip ) )
		{
            USER_ID_HANDLE = Tester->mp5k_list.size();
            Tester->mp5k_list.push_back( ID_TEMP );
		}
		else
		{
			ID_TEMP->Close();
			SAFE_DELETE( ID_TEMP );			
// write to log
#if MP5KLOG
			sprintf_s( strBuffer , "mp5k_add:[Client connect timeout]" );
			Mp5kLog.WriteLine( strBuffer );
#endif
			return MP5K_CONNECT_TIMEOUT; // id
		}
#endif

	}
// write to log
#if MP5KLOG
	sprintf_s(strBuffer, "mp5k_add:[IP = %s,Handle = %d]", ip, handle);
	Mp5kLog.WriteLine( strBuffer );
#endif
    
#if FOURPORT_CHECK
    Tester->mp5k_list.at( handle )->Send( "ROUTe:PORTs?" );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) || !SubString( ackBuffer, 2) )
	{
		return	MP5K_GET_PORT_ERROR;
	}
	isFourPort = 4 == atoi( ackBuffer.c_str() );
#endif

#if MP5KLOG
	sprintf_s(strBuffer, "mp5k_is4Port:[%d]", isFourPort);
	Mp5kLog.WriteLine( strBuffer );
#endif

    return ::MP5K_OK;
}

UINT mp5k_delete( const UINT& handle )
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle != NULL )
    {
        // notice: Watchdog close
      //  Tester->stop();
        //Sleep(10);
        usleep(10*1000);
        if ( Tester->mp5k_list.at( handle ) )
        {
#if MP5KLOG
			sprintf_s(strBuffer, "mp5k_delete:[Start Remove handle = %d]", handle);
			Mp5kLog.WriteLine( strBuffer );
#endif

            sprintf_s( strBuffer,"CONFigure:USER:REGister:REMove %d",gloMyID);
            Tester->mp5k_list.at( handle )->Send( strBuffer );
            if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
            {
                return MP5K_REMOVE_ID_TIMEOUT;
            }

            Tester->mp5k_list.at( handle )->Close();

          //  Sleep(10);
            usleep(10*1000);
            delete Tester->mp5k_list.at( handle );

#if MP5000API
            Tester->mp5k_list.at( USER_ID_HANDLE )->Close();

          //  Sleep(10);
              usleep(10*1000);
            delete Tester->mp5k_list.at( USER_ID_HANDLE );
#endif
			
        }
        // write to log
#if MP5KLOG
        sprintf_s(strBuffer, "mp5k_delete:[Remove handle = %d]", handle);
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_OK;
    }
    else
    {
        // write to log
#if MP5KLOG
        sprintf_s(strBuffer, "mp5k_delete:[Handle error]", handle);
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
}

UINT mp5k_trigger_setting( const UINT& handle , const MP5K_TriggerConfig& trigger )
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_trigger_setting:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
    Tester->gloTrigger = trigger;
    //
	 
    sprintf_s( strBuffer ,
               "CONFigure:TRIGger:TYPE %s;LEVel %.2f;OFFSet %.6f;SLICe %.8f" ,
               trigger_set[ Tester->gloTrigger.TriggerType ].c_str() ,
               Tester->gloTrigger.TriggerThreshold ,
               Tester->gloTrigger.TriggerOffset ,
               Tester->gloTrigger.TriggerGapTime
             );
	 
    Tester->mp5k_list.at( handle )->Send( strBuffer );
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_TRIGGER_TYPE_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_TRIGGER_THRESHOLD_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_TRIGGER_OFFSET_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_TRIGGER_GAPTIME_ERROR;
    }
    // write to log
#if MP5KLOG
    sprintf_s(strBuffer,
              "mp5k_trigger_setting:[Handle = %d,Type = %s,Threshold(dBm) = %.2f,Gap Time(us) = %.2f,Offset(us) = %.2f" ,
              handle ,
              trigger_set[ Tester->gloTrigger.TriggerType ].c_str() ,
              Tester->gloTrigger.TriggerThreshold , // dBm
              Tester->gloTrigger.TriggerGapTime * 1000000 , // us
              Tester->gloTrigger.TriggerOffset // us
             );
    Mp5kLog.WriteLine( strBuffer );
#endif
    return MP5K_OK;
}

UINT mp5k_correction_setting( const UINT& handle , const MP5K_CorrectionConfig& corr )
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_correction_setting:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
    Tester->gloCorrection = corr;
    sprintf_s( strBuffer,
               "CONFigure:OFDM:TRACking:FREQuency %s;:CONFigure:OFDM:TRACking:CHANnel %s" ,
               freq_est[ Tester->gloCorrection.FrequencyCorrection ].c_str() ,
               data_est[ Tester->gloCorrection.ChannelEstimation ].c_str()
             );
    Tester->mp5k_list.at( handle )->Send( strBuffer );
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_CORR_FREQUENCY_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_CORR_CHANNEL_ESTIMATION_ERROR;
    }
    // write to log
#if MP5KLOG
    sprintf_s(strBuffer, "mp5k_correction_setting:[Handle = %d,Frequency Correction = %s,Channel Estimation = %s]",
				handle ,
                freq_est[ Tester->gloCorrection.FrequencyCorrection ].c_str() ,
                data_est[ Tester->gloCorrection.ChannelEstimation ].c_str()
             );
    Mp5kLog.WriteLine( strBuffer );
#endif
    return MP5K_OK;
}

UINT mp5k_xtaltrim_setting( const UINT& handle , const MP5K_XtalTrimConfig& trim )
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_xtaltrim_setting:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
    Tester->gloXtalTrimConfig = trim;
	
	bool isSameStandard = false;
	char standard[ 12 ] = "";
	UINT OtherPort[3], cnt = 0;
	
	for(int port = 1 ;port < 5; port++)
	{
        if(port !=  Tester->gloXtalTrimConfig.UseRFPort)
		{
			OtherPort[cnt] = port;
			cnt++;
		}
	}
    Tester->mp5k_list.at( handle )->Send( "CONFigure:STANdard?" );

    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_STANDARD_ERROR;
    }

	if( boost::equals("GPRF\n", ackBuffer ) )
	{
		isSameStandard = true;
	}
	else
	{
		sprintf_s( standard , "GPRF;:" );
	}

	if( isFourPort )
	{
		sprintf_s( strBuffer ,
		           "%sROUTe:PORT:RESource RF%d,VSA;:ROUTe:PORT:RESource RF%d,OFF;:ROUTe:PORT:RESource RF%d,OFF;:ROUTe:PORT:RESource RF%d,OFF;:INPut:FREQuency %dMHZ;:CONFigure:GPRF:SPECtrum:RBW %d;:INPut:LEVel:REFerence %.2f;:INPut:CAPTure:TIME %.6f" ,
		           standard,
                   Tester->gloXtalTrimConfig.UseRFPort ,
				   OtherPort[0] ,
				   OtherPort[1] ,
				   OtherPort[2] ,
                   Tester->gloXtalTrimConfig.FreqMHz ,
                   Tester->gloXtalTrimConfig.RBW ,
                   Tester->gloXtalTrimConfig.Trim_reference - Tester->gloXtalTrimConfig.Cableloss,
				   0.001
		         );
	}
	else
	{
		sprintf_s( strBuffer ,
		           "%sROUTe:PORT:RESource RF%d,VSA;:INPut:FREQuency %dMHZ;:CONFigure:GPRF:SPECtrum:RBW %d;:INPut:LEVel:REFerence %.2f;:INPut:CAPTure:TIME %.6f" ,
				   standard,
                   Tester->gloXtalTrimConfig.UseRFPort ,
                   Tester->gloXtalTrimConfig.FreqMHz ,
                   Tester->gloXtalTrimConfig.RBW ,
                   Tester->gloXtalTrimConfig.Trim_reference - Tester->gloXtalTrimConfig.Cableloss,
				   0.001
		         );
	}
		
    Tester->mp5k_list.at( handle )->Send( strBuffer );
	if( !isSameStandard )
	{
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
			return MP5K_STANDARD_ERROR;
		}
	}

    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_USEPORT_ERROR;
    }
	if( isFourPort )
	{
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
		    return MP5K_USEPORT_ERROR;
		}
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
		    return MP5K_USEPORT_ERROR;
		}
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
		    return MP5K_USEPORT_ERROR;
		}
	}
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_TRIM_FREQUENCY_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_TRIM_RBW_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_TRIM_REF_LEVEL_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_TX_CAPTURE_LENGTH_ERROR;
    }

    Tester->mp5k_list.at( handle )->Send( "INPut:SRATe 80Mhz" );
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
	{
		return MP5K_SET_SAMPLING_RATE_ERROR;
	}

    // write to log
#if MP5KLOG
    sprintf_s( strBuffer , "mp5k_xtaltrim_setting:[Handle = %d,Standard = GPRF,Port = %d,Freq(MHz) = %d,RBW(KHz) = %d]" ,
               handle ,
               Tester->gloXtalTrimConfig.UseRFPort ,
               Tester->gloXtalTrimConfig.FreqMHz ,
               Tester->gloXtalTrimConfig.RBW / 1000 // KHz
             );
    Mp5kLog.WriteLine( strBuffer );
#endif
    return MP5K_OK;
}

UINT mp5k_xtaltrim_getoffset( const UINT& handle , double& OffsetHz )
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_xtaltrim_getoffset:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
    sprintf_s(strBuffer, "CALCulate:TXQuality:ALL:SYNC;:FETCh:ALL:POWer?");
    // Check Sync
    Tester->mp5k_list.at( handle )->Send( strBuffer );
    Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" );
	int retSync = atoi( ackBuffer.c_str() );
    // Check Poewer
    Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" );
	SubString( ackBuffer, 2);
	
	int retPower = atoi( ackBuffer.c_str() );
	double power_dBm = (double)atof( ackBuffer.c_str() );
    if ( retSync == -200 )
    {
#if MP5KLOG
        sprintf_s( strBuffer ,"mp5k_xtaltrim_getoffset:[Handle = %d,Sync failed]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_SYNC_TIMEOUT;
    }
    if ( retPower == -200)
    {
#if MP5KLOG
        sprintf_s( strBuffer ,  "mp5k_xtaltrim_getoffset:[Handle = %d,Power failed]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_MEAS_FAILED;
    }

    const double CheckPowerRef = Tester->gloXtalTrimConfig.Trim_reference - Tester->gloXtalTrimConfig.Cableloss  - 15.0;
    if ( power_dBm < CheckPowerRef )
    {
#if MP5KLOG
        sprintf_s( strBuffer ,  "mp5k_xtaltrim_getoffset:[Handle = %d,Power error = %.2f]" ,handle ,power_dBm );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_TRIM_POWER_TOO_LOW_ERROR;
    }
    sprintf_s( strBuffer , "FETCh:ALL:SPECtrum:PEAK:OFRequency?" );
    Tester->mp5k_list.at( handle )->Send( strBuffer );
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n", 5000 ) || !SubString( ackBuffer, 2) )
    {
        return MP5K_TRIM_GETOFFSET_TIMEOUT;
    }
    /*
        mp5k timeout must wait 10s
    */

	OffsetHz = ( double )atof( ackBuffer.c_str() );
    // write to log
#if MP5KLOG
    sprintf_s( strBuffer ,"mp5k_xtaltrim_getoffset:[Handle = %d,Offset(Hz) = %.2f]" , handle , OffsetHz );
    Mp5kLog.WriteLine( strBuffer );
#endif
    return MP5K_OK;
}

UINT mp5k_cal_setting( const UINT& handle , const MP5K_CalConfig& cal )
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;

    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_cal_setting:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
    Tester->gloCalConfig = cal;


    char standard[ 12 ] = "";
	bool isSameStandard = false;


    Tester->mp5k_list.at( handle )->Send( "CONFigure:STANdard?" );
	
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_STANDARD_ERROR;
    }


    if ( Tester->gloCalConfig.Standard.IsGap )
    {
        if ( Tester->gloCalConfig.Standard.type != _802_11B )
        {
            sprintf_s( standard , "WIFI:OFDM;:" );
			isSameStandard = boost::equals("OFDM\n", ackBuffer );
        }
        else
        {
            sprintf_s( standard , "WIFI:DSSS;:" );
			isSameStandard = boost::equals("DSSS\n", ackBuffer );
        }
    }
    else
    {
        sprintf_s( standard , "GPRF;:" );
		isSameStandard = boost::equals("GPRF\n", ackBuffer );

    }
	if( isSameStandard )
	{
//		ZeroMemory( standard , sizeof(standard) / sizeof( char ));
        memset( standard , 0, sizeof(standard) / sizeof( char ));
	}

	UINT OtherPort[3], cnt = 0;
	for(int port = 1 ;port < 5; port++)
	{
        if(port !=  Tester->gloCalConfig.UseRFPort)
		{
			OtherPort[cnt] = port;
			cnt++;
		}
	}
	if( isFourPort )
	{
		sprintf_s( strBuffer,
		           "%sROUTe:PORT:RESource RF%d,VSA;:ROUTe:PORT:RESource RF%d,OFF;:ROUTe:PORT:RESource RF%d,OFF;:ROUTe:PORT:RESource RF%d,OFF;:INPut:FREQuency %dMHZ;:INPut:LEVel:REFerence %.2f" ,
		           standard ,
                   Tester->gloCalConfig.UseRFPort ,
				   OtherPort[0] ,
				   OtherPort[1] ,
				   OtherPort[2] ,
                   Tester->gloCalConfig.FreqMHz ,
                   Tester->gloCalConfig.RefLeveldBm
		         );
	}
	else
	{
		sprintf_s( strBuffer,
		           "%sROUTe:PORT:RESource RF%d,VSA;:INPut:FREQuency %dMHZ;:INPut:LEVel:REFerence %.2f" ,
		           standard ,
                   Tester->gloCalConfig.UseRFPort ,
                   Tester->gloCalConfig.FreqMHz ,
                   Tester->gloCalConfig.RefLeveldBm
		         );
	}
    Tester->mp5k_list.at( handle )->Send( strBuffer );
	if( !isSameStandard )
	{
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
			return MP5K_STANDARD_ERROR;
		}
	}
    
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_USEPORT_ERROR;
    }
	if( isFourPort )
	{
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
		    return MP5K_USEPORT_ERROR;
		}
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
		    return MP5K_USEPORT_ERROR;
		}
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
		    return MP5K_USEPORT_ERROR;
		}
	}
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_CAL_FREQUENCY_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_CAL_REF_LEVEL_ERROR;
    }

    if ( Tester->gloCalConfig.Standard.type != _802_11B )
    {
        if( Tester->gloCalConfig.Standard.OFDM.Modulation.Bandwidth == _80M )
		{
			sprintf_s( strBuffer, "INPut:SRATe 160Mhz");
		}	
		else
		{
			sprintf_s( strBuffer, "INPut:SRATe 80Mhz");
		}
        Tester->mp5k_list.at( handle )->Send( strBuffer );
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
			return MP5K_SET_SAMPLING_RATE_ERROR;
		}
    }

    // write to log
#if MP5KLOG
    sprintf_s( strBuffer ,"mp5k_cal_setting:[Handle = %d,Standard = %d,Port = %d,Freq(MHz) = %d,Ref level(dBm) = %.2f]" ,
               handle ,
               Tester->gloCalConfig.Standard.type,
               Tester->gloCalConfig.UseRFPort,
               Tester->gloCalConfig.FreqMHz,
               Tester->gloCalConfig.RefLeveldBm
             );
    Mp5kLog.WriteLine( strBuffer );
#endif
    return MP5K_OK;
}

UINT mp5k_cal_meas(const UINT& handle, double& PowerdBm)
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_cal_meas:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
    if ( Tester->gloCalConfig.Standard.IsGap )
    {
        if ( Tester->gloCalConfig.Standard.type != _802_11B )
		{
			sprintf_s( strBuffer , "CALCulate:TXQuality:ALL:SYNC;:FETCh:SEGMent1:OFDM:POWer?" );//;EVM?;PERRor?;FERRor?;SCERror?;LOLeakage?;MASK?
		}
		else
		{
			sprintf_s( strBuffer , "CALCulate:TXQuality:ALL:SYNC;:FETCh:SEGMent1:DSSS:POWer?" ); //;EVM:ALL?;:FETCh:SEGMent1:DSSS:PERRor?;FERRor?;SCERror?;LOLeakage?;MASK?
		}
	}
	else
	{
		sprintf_s( strBuffer, "CALCulate:TXQuality:ALL:SYNC;:FETCh:SEGMent1:POWer?" );
	}
    
    Tester->mp5k_list.at( handle)->Send( strBuffer );
    // Sync
    Tester->mp5k_list.at( handle)->IsWaitTillReadToken( ackBuffer , "\n" );
	int retSync = atoi( ackBuffer.c_str() );
    // Power
    Tester->mp5k_list.at( handle)->IsWaitTillReadToken( ackBuffer , "\n" );
	SubString( ackBuffer, 2);
	int retPower = atoi( ackBuffer.c_str() );
	PowerdBm = ( double )atof( ackBuffer.c_str() );
    // Check Sync
    if ( retSync == -200 )
    {
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_cal_meas:[Handle = %d,Sync failed]" ,handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_SYNC_TIMEOUT;
    }
    if ( retPower == -200)
    {
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_cal_meas:[Handle = %d,Power failed]" ,handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_MEAS_FAILED;
    }
    // write to log
#if MP5KLOG
    sprintf_s( strBuffer , "mp5k_cal_meas:[Handle = %d,Power(dBm)= %.2f]" ,handle , PowerdBm );
    Mp5kLog.WriteLine( strBuffer );
#endif
    return MP5K_OK;
}

UINT mp5k_tx_setting( const UINT& handle , const MP5K_TxTestConfig& txtest )
{
	 
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_tx_setting:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
    Tester->gloTXConfig = txtest;
	char standard[ 12 ] = {"/0"};


    if( Tester->gloTXConfig.isBeamforming )
	{
		MP5K_RxTestConfig rxconfig;
        rxconfig.FreqMHz = Tester->gloTXConfig.FreqMHz;
		rxconfig.OutPowerdBm = -60.00;
		rxconfig.PacketCount = 100;
        rxconfig.Standard = Tester->gloTXConfig.Standard;
        rxconfig.UseRFPort = Tester->gloTXConfig.UseRFPort;

		mp5k_rx_setting( handle, rxconfig );
		mp5k_rx_start( handle );
	}
	

    Tester->mp5k_list.at( handle )->Send( "CONFigure:STANdard?" );
	
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_STANDARD_ERROR;
    }

	bool isSameStandard = false;

    if ( Tester->gloTXConfig.Standard.type != _802_11B )
    {
        sprintf_s( standard , "WIFI:OFDM;:" );
		isSameStandard = boost::equals("OFDM\n", ackBuffer );
    }
    else
    {
        sprintf_s( standard , "WIFI:DSSS;:" );
		isSameStandard = boost::equals("DSSS\n", ackBuffer );
    }
	
	if( isSameStandard )
	{
        //ZeroMemory( standard , sizeof(standard) / sizeof( char ));  allen
        memset( standard , 0, sizeof(standard) / sizeof( char ));
	}

	UINT OtherPort[3], cnt = 0;
	for(int port = 1 ;port < 5; port++)
	{
        if(port !=  Tester->gloTXConfig.UseRFPort)
		{
			OtherPort[cnt] = port;
			cnt++;
		}
	}

	if( isFourPort )
	{
		sprintf_s( strBuffer ,
		           "%sROUTe:PORT:RESource RF%d,VSA;:ROUTe:PORT:RESource RF%d,OFF;:ROUTe:PORT:RESource RF%d,OFF;:ROUTe:PORT:RESource RF%d,OFF;:INPut:FREQuency %dMHZ;:INPut:LEVel:REFerence %.2f;:INPut:CAPTure:TIME %.6f" ,
		           standard ,
                   Tester->gloTXConfig.UseRFPort ,
				   OtherPort[0] ,
				   OtherPort[1] ,
				   OtherPort[2] ,
                   Tester->gloTXConfig.FreqMHz ,
                   Tester->gloTXConfig.RefLeveldBm ,
                   Tester->gloTXConfig.CaptureLength
		         );
	}
	else
	{
		 
		sprintf_s( strBuffer ,
		           "%sROUTe:PORT:RESource RF%d,VSA;:INPut:FREQuency %dMHZ;:INPut:LEVel:REFerence %.2f;:INPut:CAPTure:TIME %.6f" ,
		           standard ,
                   Tester->gloTXConfig.UseRFPort ,
                   Tester->gloTXConfig.FreqMHz ,
                   Tester->gloTXConfig.RefLeveldBm ,
                   Tester->gloTXConfig.CaptureLength
				   );
		 
	}

    Tester->mp5k_list.at( handle )->Send( strBuffer );
	
	if( !isSameStandard )
	{
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
			return MP5K_STANDARD_ERROR;
		}
	}
	

    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_USEPORT_ERROR;
    }
	if( isFourPort )
	{
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
		    return MP5K_USEPORT_ERROR;
		}
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
		    return MP5K_USEPORT_ERROR;
		}
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
		    return MP5K_USEPORT_ERROR;
		}
	}
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_TX_FREQUENCY_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_TX_REF_LEVEL_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_TX_CAPTURE_LENGTH_ERROR;
    }

    
    if ( Tester->gloTXConfig.Standard.type != _802_11B )
    {
        if( Tester->gloTXConfig.Standard.OFDM.Modulation.Bandwidth == _20M )
		{
			sprintf_s( strBuffer, "INPut:SRATe 80Mhz");
		}	
		else
		{
			sprintf_s( strBuffer, "INPut:SRATe 160Mhz");
		}
        Tester->mp5k_list.at( handle )->Send( strBuffer );
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
			return MP5K_SET_SAMPLING_RATE_ERROR;
		}
    }

    Tester->mp5k_list.at( handle )->Send( "CONFigure:STANdard?" );
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_STANDARD_ERROR;
    }
    if ( Tester->gloTXConfig.Standard.type != _802_11B )
    {
        sprintf_s( standard , "WIFI:OFDM" );
		isSameStandard = boost::equals("OFDM\n", ackBuffer );
    }
    else
    {
        sprintf_s( standard , "WIFI:DSSS" );
		isSameStandard = boost::equals("DSSS\n", ackBuffer );
    }
	if( !isSameStandard )
	{
        Tester->mp5k_list.at( handle )->Send( standard );
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
			return MP5K_STANDARD_ERROR;
		}
	}

    Tester->mp5k_list.at( handle )->Send( "INPut:STARt" );
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_MEAS_TIME_OUT;
    }
    
#if MP5KLOG
    sprintf_s( strBuffer ,
               "mp5k_tx_setting:[Handle = %d,Standard = %d,Port = %d,Freq(MHz)=%d,Ref level(dBm)= %.2f,Capture length(ms)=%.3f]" ,
               handle ,
               Tester->gloTXConfig.Standard.type,
               Tester->gloTXConfig.UseRFPort,
               Tester->gloTXConfig.FreqMHz,
               Tester->gloTXConfig.RefLeveldBm,
               Tester->gloTXConfig.CaptureLength * 1000 // ms
             );
    Mp5kLog.WriteLine( strBuffer );
#endif
	isWifi_TX_Sync = false;
    return MP5K_OK;
}

UINT mp5k_tx_meas( const UINT& handle , MP5K_TxTestMeas& txmeas )
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_tx_meas:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
    // Mode
    char standard[ BUFFER_LENGTH ];
    char evm[ BUFFER_LENGTH ];
    if ( Tester->gloTXConfig.Standard.type != _802_11B )
    {
        sprintf_s( standard , "OFDM" );
        sprintf_s( evm , "EVM?" );
        sprintf_s( strBuffer , "CALCulate:TXQuality:ALL:SYNC;:FETCh:SEGMent1:OFDM:POWer?" );//;EVM?;PERRor?;FERRor?;SCERror?;LOLeakage?;MASK?
    }
    else
    {
        sprintf_s( standard , "DSSS" );
        sprintf_s( evm , "EVM:ALL?" );
        sprintf_s( strBuffer , "CALCulate:TXQuality:ALL:SYNC;:FETCh:SEGMent1:DSSS:POWer?" ); //;EVM:ALL?;:FETCh:SEGMent1:DSSS:PERRor?;FERRor?;SCERror?;LOLeakage?;MASK?
    }
	std::string Sync;
	std::string Power;
    
    Tester->mp5k_list.at( handle )->Send( strBuffer );
    bool IsSync = Tester->mp5k_list.at( handle )->IsWaitTillReadToken( Sync , "\n" );
    bool IsPower = Tester->mp5k_list.at( handle )->IsWaitTillReadToken( Power , "\n" );
	/* allen 20170620
    Tester->mp5k_list.at( handle )->Send( "CONFigure:TRIGger:TYPE IMMediate" );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_TRIGGER_TYPE_ERROR;
    }
	*/
    if ( IsSync )
    {
        /*--------------------------------------------------------------------------------------------------------------------------------*/
        // Check Sync
		int retSync = atoi( Sync.c_str() );
        if ( retSync == -200 )
        {
#if MP5KLOG
            sprintf_s( strBuffer , "mp5k_tx_meas:[Handle = %d,Sync failed]" , handle );
            Mp5kLog.WriteLine( strBuffer );
#endif
			std::string CaptureCmd = "CAPTure:STORe Sync failed_" + boost::lexical_cast<std::string>( gloMyID ) + ".wfm";
            Tester->mp5k_list.at( handle )->Send( CaptureCmd );


            Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" );
            return MP5K_SYNC_TIMEOUT;
            /*--------------------------------------------------------------------------------------------------------------------------------*/
        }
        else
        {
            /*--------------------------------------------------------------------------------------------------------------------------------*/
            // Get Power, Phase Error, Frequency Error, Symbol Clock Error, Lo Leakage, MASK, EVM
            if ( !IsPower )
            {
#if MP5KLOG
                sprintf_s( strBuffer , "mp5k_tx_meas:[Handle = %d,Power measurement timeout]" , handle );
                Mp5kLog.WriteLine( strBuffer );
#endif
                return MP5K_MEAS_TIME_OUT;
            }
			int retPower = atoi( Power.c_str() );
            if ( retPower == -200)
            {
				std::string CaptureCmd = "CAPTure:STORe Power failed_" + boost::lexical_cast<std::string>( gloMyID ) + ".wfm";
                Tester->mp5k_list.at( handle )->Send( CaptureCmd );
                Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" );
#if MP5KLOG
                sprintf_s( strBuffer ,"mp5k_tx_meas:[Handle = %d,Power failed]" ,handle);
                Mp5kLog.WriteLine( strBuffer );
#endif
                return MP5K_MEAS_FAILED;
            }
			Power = Power.substr(2, Power.size() );
			txmeas.TXPowerdBm = atof( Power.c_str() );
            std::string EVM;
            std::string Phase;
            std::string Frequency;
            std::string Symbol;
            std::string Lo;
            std::string MASK;
			std::string MASKERROR;
			std::string ISBeamformingON;
			std::string BeamformingPower;

			sprintf_s( strBuffer , "FETCh:SEGMent1:%s:PERRor?;FERRor?;SCERror?;LOLeakage?;MASK?;%s" , standard , evm );
            Tester->mp5k_list.at( handle )->Send( strBuffer );
            if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( Phase , "\n" ) )       // Phase Error
            {
                return MP5K_TX_READ_PHASE_ERROR;
            }
            if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( Frequency , "\n" ) )   // Frequency Error
            {
                return MP5K_TX_READ_FREQUENCY_ERROR;
            }
            if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( Symbol , "\n" ) )      // Symbol Clock Error
            {
                return MP5K_TX_READ_SYMBOL_CLOCK_ERROR;
            }
            if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( Lo , "\n" ) )          // Lo Leakage
            {
                return MP5K_TX_READ_LO_LEAKAGE_ERROR;
            }
            if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( MASK , "\n" ) )        // MASK
            {
                return MP5K_TX_READ_MASK_ERROR;
            }
            if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( EVM , "\n" ) )         // EVM
            {
                return MP5K_TX_READ_EVM_ERROR;
            }

			int retEVM = atoi( EVM.c_str() );
			if ( retEVM == -200 )
			{
#if MP5KLOG
				sprintf_s( strBuffer , "mp5k_tx_meas:[Handle = %d,EVM failed]" , handle );
				Mp5kLog.WriteLine( strBuffer );
#endif
				std::string CaptureCmd = "CAPTure:STORe EVM failed_" + boost::lexical_cast<std::string>( gloMyID ) + ".wfm";
                Tester->mp5k_list.at( handle )->Send( CaptureCmd );
           
                Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" );
				return MP5K_TX_READ_EVM_ERROR;
            /*--------------------------------------------------------------------------------------------------------------------------------*/
			}



			sprintf_s( strBuffer , "FETCh:SEGMent1:%s:MASK:VIOLation?" , standard );
            Tester->mp5k_list.at( handle )->Send( strBuffer );
			
            if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( MASKERROR , "\n" ) )       // Phase Error
            {
                return MP5K_TX_READ_MASK_ERROR_ERROR;
            }
			
			/***Add BF Meas******/
            if( Tester->gloTXConfig.isBeamforming )
			{				
                Tester->mp5k_list.at( handle )->Send("FETCh:SEGMent1:OFDM:BEAMformed?");
                if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ISBeamformingON , "\n" ) )       // Phase Error
				{
					return MP5K_TX_READ_MASK_ERROR_ERROR;
				}
				ISBeamformingON = ISBeamformingON.substr(2 , ISBeamformingON.size() );
				int BFON = atoi( ISBeamformingON.c_str() );
				if (BFON == 1)
				{
					txmeas.isBFON = true;
					
                    Tester->mp5k_list.at( handle )->Send("CALCulate:TXQuality:ALL:SYNC;:FETCh:SEGMent1:OFDM:POWer?");
                    IsSync = Tester->mp5k_list.at( handle )->IsWaitTillReadToken( Sync , "\n" );
                    IsPower = Tester->mp5k_list.at( handle )->IsWaitTillReadToken( Power , "\n" );
					if ( IsSync || IsPower )
					{
						//Power = Power.substr(2 , Power.size() );
						SubString( Power, 2 );
						txmeas.PreamblePower = atof( Power.c_str() );
					}
					else
					{
						txmeas.PreamblePower = 0.00;
						return MP5K_MEAS_TIME_OUT;
					}
				}
				else
				{
					txmeas.isBFON = false;
					txmeas.PreamblePower = 0.00;
				}
			}

			std::vector<std::string> returnData;
			SubString( MASKERROR, 2 );
            //boost::split( returnData, std::string(MASKERROR) , boost::is_any_of(","));
            boost::split( returnData, (MASKERROR) , boost::is_any_of(",")); // allen
			if( returnData.size() < 2 )
			{
				return MP5K_TX_READ_MASK_ERROR_ERROR;
			}
			txmeas.MaskErrorCount = atof(returnData.at(1).c_str());
			txmeas.MaskTotalCount = atof(returnData.at(0).c_str());
			
			/***Add Mask Error******/

			SubString( EVM, 2 );
			txmeas.EVM = atof( EVM.c_str() );
			

			SubString( Phase, 2 );
            txmeas.PhaseError = atof( Phase.c_str() );
            
	
			SubString( Frequency, 2 );
            txmeas.FrequencyError = atof( Frequency.c_str() );

	
			SubString( Symbol, 2 );
            txmeas.SymbolClockError = atof( Symbol.c_str() );

			SubString( Lo, 2 );
            txmeas.LoLeakage = atof( Lo.c_str() );
           
			SubString( MASK, 2 );
            int mask = atoi( MASK.c_str() );
            if (mask == 0)
            {
                txmeas.MASK = true;
            }
            else
            {
                txmeas.MASK = false;
            }

#if MP5KLOG
            sprintf_s( strBuffer ,
                       "mp5k_tx_meas: Handle=%d,TX Power(dBm)=%.2f,EVM=%.2f,Phase Error=%.2f,Frequency Error=%.2f,Symbol Clock Error=%.2f,Lo Leakage=%.2f,MASK=%d,MASK_Error=%d,MASK_ErrorTotal=%d" ,
                       handle ,
                       txmeas.TXPowerdBm ,
                       txmeas.EVM ,
                       txmeas.PhaseError ,
                       txmeas.FrequencyError ,
                       txmeas.SymbolClockError ,
                       txmeas.LoLeakage ,
                       txmeas.MASK,
					   txmeas.MaskErrorCount,
					   txmeas.MaskTotalCount
                     );
            Mp5kLog.WriteLine( strBuffer );
#endif
			isWifi_TX_Sync = true;
            return MP5K_OK;
            /*--------------------------------------------------------------------------------------------------------------------------------*/
        }
    }
    else
    {
        /*--------------------------------------------------------------------------------------------------------------------------------*/
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_tx_meas:[Handle = %d, Sync timeout]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_MEAS_TIME_OUT;
        /*--------------------------------------------------------------------------------------------------------------------------------*/
    }
}

UINT mp5k_rx_setting( const UINT& handle , const MP5K_RxTestConfig& rxtest )
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_rx_setting:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
    Tester->gloRXConfig = rxtest;
    UINT uTime = 0;
    // Standard
    char _standard[ 12 ];
    if ( Tester->gloRXConfig.Standard.type != _802_11B )
    {
        sprintf_s( _standard , "WIFI:OFDM" );
    }
    else
    {
        sprintf_s( _standard , "WIFI:DSSS" );
    }
    // Wave Form
    char standard[ BUFFER_LENGTH ];
    char bandwidth[ BUFFER_LENGTH ];
    char rate[ BUFFER_LENGTH ];
	char Path[ BUFFER_LENGTH ];
    // standard & bandwidth & rate
    switch( Tester->gloRXConfig.Standard.type )
    {
    case _802_11AC:        sprintf_s( standard , "ac" );
    case _802_11N:
        if( Tester->gloRXConfig.Standard.type != _802_11AC )
        {
            sprintf_s( standard , "n" );
        }
        switch( Tester->gloRXConfig.Standard.OFDM.Modulation.Bandwidth )
        {
        case _20M:            sprintf_s( bandwidth , "20" );            break;
        case _40M:            sprintf_s( bandwidth , "40" );            break;
        case _80M:            sprintf_s( bandwidth , "80" );            break;
        default:            return MP5K_RX_SET_BANDWIDTH_ERROR;
        }
        // 11ac/n
        switch( Tester->gloRXConfig.Standard.OFDM.Modulation.rate )
        {
        case MCS0:            sprintf_s( rate , "MCS0" );            break;
        case MCS1:            sprintf_s( rate , "MCS1" );            break;
        case MCS2:            sprintf_s( rate , "MCS2" );            break;
        case MCS3:            sprintf_s( rate , "MCS3" );            break;
        case MCS4:            sprintf_s( rate , "MCS4" );            break;
        case MCS5:            sprintf_s( rate , "MCS5" );            break;
        case MCS6:            sprintf_s( rate , "MCS6" );            break;
        case MCS7:            sprintf_s( rate , "MCS7" );            break;
        case MCS8:            sprintf_s( rate , "MCS8" );            break;
        case MCS9:            sprintf_s( rate , "MCS9" );            break;
        default:            return MP5K_RX_SET_RATE_ERROR;
        }
        break;
    case _802_11A:
    case _802_11G:
        sprintf_s( standard , "ag" );
        switch( Tester->gloRXConfig.Standard.OFDM.Modulation.Bandwidth )
        {
        case _20M:
            sprintf_s( bandwidth , "20" );
            break;
        default:
            return MP5K_RX_SET_BANDWIDTH_ERROR;
        }
        switch( Tester->gloRXConfig.Standard.OFDM.Modulation.rate )
        {
        // 11a/g
        case OFDM_6M:            sprintf_s( rate , "6M" );            break;
        case OFDM_9M:            sprintf_s( rate , "9M" );            break;
        case OFDM_12M:            sprintf_s( rate , "12M" );            break;
        case OFDM_18M:            sprintf_s( rate , "18M" );            break;
        case OFDM_24M:            sprintf_s( rate , "24M" );            break;
        case OFDM_36M:            sprintf_s( rate , "36M" );            break;
        case OFDM_48M:            sprintf_s( rate , "48M" );            break;
        case OFDM_54M:            sprintf_s( rate , "54M" );            break;
        default:
            return MP5K_RX_SET_RATE_ERROR;
        }
        break;
    case _802_11B:
        sprintf_s( standard , "b" );
        sprintf_s( bandwidth , "20" );
        switch( Tester->gloRXConfig.Standard.DSSS.rate )
        {
        case CCK_1M:            sprintf_s( rate , "1M" );            break;
        case CCK_2M:            sprintf_s( rate , "2M" );            break;
        case CCK_55M:            sprintf_s( rate , "5.5M" );            break;
        case CCK_11M:            sprintf_s( rate , "11M" );            break;
        default:
            return MP5K_RX_SET_RATE_ERROR;
        }
        break;
    }
    if ( Tester->gloRXConfig.PacketCount < 0 )
    {
        return MP5K_RX_PACKET_COUNT_ERROR;
    }
    // MP5000 Waveform = \MP\WIFI\11[STANDARD]_[BANDWIDTH]_[RATE].mp5000vsg
	UINT OtherPort[3], cnt = 0;
	for(int port = 1 ;port < 5; port++)
	{
    if(port !=  Tester->gloRXConfig.UseRFPort)
		{
			OtherPort[cnt] = port;
			cnt++;
		}
	}
	sprintf_s( strBuffer , "11%s_%s_%s"  ,standard , bandwidth , rate );
	std::string fileTemp = strBuffer;
	if( WaveformQueue.find( fileTemp + ".wfm" ) != WaveformQueue.end() )
	{
		sprintf_s( Path , "\\USER\\Port%i\\", gloMyID );
		fileTemp = fileTemp + ".wfm" ;
	}
	else if( WaveformQueue.find( fileTemp + ".mp5000vsg" ) != WaveformQueue.end() )
	{
		sprintf_s( Path , "\\USER\\Port%i\\", gloMyID );
		fileTemp = fileTemp + ".mp5000vsg" ;
	}
	else
	{
		sprintf_s( Path , "\\MP\\WIFI\\" );
		fileTemp = fileTemp + ".mp5000vsg" ;
	}

	if( isFourPort )
	{
		sprintf_s( strBuffer ,
		           "%s;:ROUTe:PORT:RESource RF%d,VSG;:ROUTe:PORT:RESource RF%d,OFF;:ROUTe:PORT:RESource RF%d,OFF;:ROUTe:PORT:RESource RF%d,OFF;:OUTPut:FREQuency %dMHz;:OUTPut:POWer:LEVel %.2f;:OUTPut:LOOP:COUNt %d;:OUTPut:WAVeform:LOAD %s%s" ,
		           _standard ,
                   Tester->gloRXConfig.UseRFPort ,
				   OtherPort[0] ,
				   OtherPort[1] ,
				   OtherPort[2] ,
                   Tester->gloRXConfig.FreqMHz ,
                   Tester->gloRXConfig.OutPowerdBm ,
                   Tester->gloRXConfig.PacketCount ,
				   Path,
				   fileTemp.c_str()
		         );
	}
	else
	{
		sprintf_s( strBuffer ,
		           "%s;:ROUTe:PORT:RESource RF%d,VSG;:OUTPut:FREQuency %dMHz;:OUTPut:POWer:LEVel %.2f;:OUTPut:LOOP:COUNt %d;:OUTPut:WAVeform:LOAD %s%s" ,
		           _standard ,
                   Tester->gloRXConfig.UseRFPort ,
                   Tester->gloRXConfig.FreqMHz ,
                   Tester->gloRXConfig.OutPowerdBm ,
                   Tester->gloRXConfig.PacketCount ,
				   Path,
		           fileTemp.c_str()
		         );
	}

    Tester->mp5k_list.at( handle )->Send( strBuffer );
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_STANDARD_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_USEPORT_ERROR;
    }
	if( isFourPort )
	{
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
		    return MP5K_USEPORT_ERROR;
		}
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
		    return MP5K_USEPORT_ERROR;
		}
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
		    return MP5K_USEPORT_ERROR;
		}
	}
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_RX_FREQUENCY_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_RX_OUTPUT_POWER_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_RX_PACKET_COUNT_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_RX_WAVEFORM_ERROR;
    }
	int failure = atoi( ackBuffer.c_str() ); // Allen 20170628 begin
	if (failure)
	{  
		return MP5K_RX_WAVEFORM_ERROR;
	}                                         // Allen 20170628 end
    // write to log
#if MP5KLOG
    sprintf_s( strBuffer ,
               "mp5k_rx_setting:[Handle = %d,Waveform = \\MP\\WIFI\\11%s_%s_%s.mp5000vsg,Output freq(Mhz) = %d,Output level(dBm) = %.2f,Packet count = %d" ,
               handle ,
               standard ,
               bandwidth ,
               rate ,
               rxtest.FreqMHz ,
               rxtest.OutPowerdBm ,
               rxtest.PacketCount
             );
    Mp5kLog.WriteLine( strBuffer );
#endif
    return MP5K_OK;
}

UINT mp5k_rx_start(const UINT& handle)
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_rx_start:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
	if( WIFI_RX_ASYNC_MODE == RX_START_SYNC )
	{
		sprintf_s( strBuffer , "OUTPut:STARt" );
	}
	else
	{
		sprintf_s( strBuffer , "OUTPut:STARt:ASYNchronous" );
	}
    Tester->mp5k_list.at( handle )->Send( strBuffer );
    // Check rx timeout
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" , rx_time_out ))
    {
        //mp5k tester refresh or clear
        Tester->mp5k_list.at( handle )->Close();
        Tester->mp5k_list.at( handle )->Open( Tester->mp5k_list.at( handle )->ip , 5499 , 2 );
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_rx_start:[Handle = %d,Timeout]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_RX_START_TIMEOUT;
    }
    // write to log
#if MP5KLOG
    sprintf_s( strBuffer , "mp5k_rx_start:[Handle = %d,Started]" , handle );
    Mp5kLog.WriteLine( strBuffer );
#endif
    return MP5K_OK;
}

UINT mp5k_set_dutid( const UINT& handle , const UINT& id )
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_set_dutid:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
    //Set next DUT Id
    sprintf_s( strBuffer , "CONFigure:USER %d" , id );
    Tester->mp5k_list.at( handle )->Send( strBuffer );
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_MEAS_TIME_OUT;
    }
    // write to log
#if MP5KLOG
    sprintf_s(strBuffer, "mp5k_set_dutid:[Handle = %d,Current ID = %d", handle , id );
    Mp5kLog.WriteLine( strBuffer );
#endif
    return MP5K_OK;
}

#if MULTIDUT
UINT mp5k_check_id( const UINT& handle , const UINT& id , const UINT& timeout_ms )
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_check_id:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
    boost::timer timer1;
    double timer_count;
    UINT uId = 0;
    do
    {
        sprintf_s( strBuffer , "CONFigure:USER?" );
        Tester->mp5k_list.at( handle )->Send( strBuffer );
        if ( Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) == false )
        {
            // write to log
#if MP5KLOG
            sprintf_s( strBuffer , "mp5k_check_id:[Timeout]" , handle );
            Mp5kLog.WriteLine( strBuffer );
#endif
            return MP5K_CHECK_ID_TIMEOUT;
        }
		if( SubString( ackBuffer ,2 ) )
		{
			uId = (UINT)atoi( ackBuffer.c_str() );
			if ( uId == id )
			{
				// write to log
#if MP5KLOG
				sprintf_s( strBuffer , "mp5k_check_id:[Handle = %d,OK]" , handle );
				Mp5kLog.WriteLine( strBuffer );
#endif
				return MP5K_OK;
			}
		}
		
        timer_count = timer1.elapsed() * 1000;
    }
    while ( timer_count < timeout_ms );
    // write to log
#if MP5KLOG
    sprintf_s( strBuffer , "mp5k_check_id:[Timeout]" , handle );
    Mp5kLog.WriteLine( strBuffer );
#endif
    return MP5K_CHECK_ID_TIMEOUT_BY_CUSTOM;
}

UINT mp5k_multi_dut( const bool& Enable )
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    Tester->MultiDUTEnable = Enable;
    // write to log
#if MP5KLOG
    sprintf_s( strBuffer , "mp5k_multi_dut:[MultiDUT = %d]" , Enable );
    Mp5kLog.WriteLine( strBuffer );
#endif
    return MP5K_OK;
}

UINT mp5k_change_id( const UINT& MyID , const UINT& NextID )
{
    gloMyID = MyID;
    gloNextID = NextID;
    // write to log
#if MP5KLOG
    sprintf_s( strBuffer , "mp5k_change_id:[MyID = %d, NextID = %d]" , gloMyID , gloNextID );
    Mp5kLog.WriteLine( strBuffer );
#endif
	//sprintf_s( strBuffer , "D:\\Adivic\\MP5000 Tester\\mpvsg\\USER\\Port%u\\*.*" , gloMyID  );
	
    return MP5K_OK;
}

UINT mp5k_wait_server_ready( const UINT& handle , const UINT& Milliseconds )
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_wait_server_ready:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
    if ( Tester->MultiDUTEnable )
    {
//        sprintf_s( strBuffer , "CONFigure:USER?" );
        Tester->mp5k_list.at( handle )->Send( "CONFigure:USER?" );
        if ( Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) == false )
        {
            // write to log
#if MP5KLOG
            sprintf_s( strBuffer , "mp5k_wait_server_ready:[Timeout]" , handle );
            Mp5kLog.WriteLine( strBuffer );
#endif
            return MP5K_CHECK_ID_TIMEOUT;
        }
        
		SubString( ackBuffer, 2 );
		UINT uId = (UINT)atoi( ackBuffer.c_str() );
        if ( uId == gloMyID )
        {
            // write to log
#if MP5KLOG
            sprintf_s( strBuffer , "mp5k_wait_server_ready:[Handle = %d,OK]" , handle );
            Mp5kLog.WriteLine( strBuffer );
#endif
            return MP5K_OK;
        }
        else
        {
            return MP5K_CHECK_ID_INCONSISTENT;
        }
    }
    return MP5K_OK;
}

UINT mp5k_release_lock( const UINT& handle )
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_release_lock:[Handle error]" );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
    if ( Tester->MultiDUTEnable )
    {
        if ( mp5k_set_dutid( handle , gloNextID ) == MP5K_OK )
        {
            // write to log
#if MP5KLOG
            sprintf_s( strBuffer , "mp5k_release_lock:[Handle = %d, NextID = %d]" , handle , gloNextID );
            Mp5kLog.WriteLine( strBuffer );
#endif
            return MP5K_OK;
        }
        else
        {
            // write to log
#if MP5KLOG
            sprintf_s( strBuffer , "mp5k_release_lock:[Timeout]" );
            Mp5kLog.WriteLine( strBuffer );
#endif
            return MP5K_WAIT_ID_FAILED;
        }
    }
    return MP5K_OK;
}

UINT mp5k_measure_done( const UINT& handle )
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( Tester->MultiDUTEnable )
    {
        // watchdog thread run
        sprintf_s( strBuffer,"CONFigure:USER:REGister:STORe %d",gloMyID);
        Tester->mp5k_list.at( handle )->Send( strBuffer );
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
        {
            return MP5K_REGISTER_ID_TIMEOUT;
        }
        Tester->Handle = handle;
      //  Tester->start_for_run();   // allen
        //Sleep(1); // 1 time check id
        usleep(1*1000);
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_measure_done:[Handle = %d,WatchDog starting]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_OK;
    }
    return MP5K_OK;
}

UINT mp5k_watchdog_stop( const UINT& handle )
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( Tester->MultiDUTEnable )
    {
      //  Tester->abort();  // allen
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_watchdog_stop:[Handle = %d,WatchDog abort]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_OK;
    }
    // write to log
#if MP5KLOG
    sprintf_s( strBuffer , "mp5k_watchdog_stop:[Handle = %d,No Multi-DUT]" , handle );
    Mp5kLog.WriteLine( strBuffer );
#endif
    return MP5K_OK;
}
#endif
UINT mp5k_close( const UINT& handle )
{
    if ( Tester != nullptr )
    {
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_close:[Success,WatchDog Closed]" );
        Mp5kLog.WriteLine( strBuffer );
#endif
		

       // Sleep( 10 );
        usleep(10 *1000);
        delete Tester;
		Tester = nullptr;
		ResetSub();
    }
	
    // write to log
    return MP5K_OK;
}

UINT mp5k_get_error_message( const UINT& code , std::string& strRet )
{
    // int size = ( int ) MP5K_RX_TIME_OUT + 1; //notice:last+1
    strRet = MessageTable[ code ];
    return MP5K_OK;
}

UINT mp5k_tx_meas_extend( const UINT& handle , MP5K_TxTestMeasExtend& txmeasext )
{
    if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL)
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_tx_meas:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
	char standard[ BUFFER_LENGTH ] = { 0 };
    if ( Tester->gloTXConfig.Standard.type != _802_11B )
    {
        sprintf_s( standard , "OFDM" );
    }
    else
    {
        sprintf_s( standard , "DSSS" );
    }

	std::string Sync;
    
    bool IsSync = false;

    if( Tester->gloTXConfig.Standard.type == _802_11AC && Tester->gloTXConfig.Standard.OFDM.Modulation.Bandwidth == _80M )
	{
		isWifi_TX_Sync = false;
        Tester->mp5k_list.at( handle )->Send( "CONFigure:OFDM:SPECtrum:WIDE ON" );
        if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
			return MP5K_TX_SET_WIDE_ERROR;
		}
	}

	if( isWifi_TX_Sync )
	{
		IsSync = true;
	}
	else
	{
        Tester->mp5k_list.at( handle )->Send( "CALCulate:TXQuality:ALL:SYNC" );
        IsSync = Tester->mp5k_list.at( handle )->IsWaitTillReadToken( Sync , "\n" );
	}
  
    if( IsSync )
    {
        int retSync = atoi( Sync.c_str() );
        if ( retSync == -200 )
        {
#if MP5KLOG
            sprintf_s( strBuffer , "mp5k_tx_meas:[Handle = %d,Sync failed]" , handle );
            Mp5kLog.WriteLine( strBuffer );
#endif
			std::string CaptureCmd = "CAPTure:STORe Sync failed_" + boost::lexical_cast<std::string>( gloMyID ) + ".wfm";
            Tester->mp5k_list.at( handle )->Send( CaptureCmd );
           
            Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" );
            return MP5K_SYNC_TIMEOUT;
            /*--------------------------------------------------------------------------------------------------------------------------------*/
        }
        else
        {
            std::string FLATNESS;
            std::string RAMP_ON;
            std::string RAMP_OFF;
            std::string MARGIN;
            std::string FREQ;
            std::vector<std::string> tempMargin;
            std::vector<std::string> tempFreq;

            sprintf_s(strBuffer,"FETCh:SEGMent1:%s:MASK:MARGin?;OFRequency?",standard);
            Tester->mp5k_list.at( handle )->Send( strBuffer );
            if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( MARGIN , "\n",2 ) )
            {
                return MP5K_TXEX_READ_MARGIN_ERROR;
            }
            if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( FREQ , "\n", 2 ) )
            {
                return MP5K_TXEX_READ_FREQ_ERROR;
            }
            if ( Tester->gloTXConfig.Standard.type != _802_11B )
            {
                Tester->mp5k_list.at( handle )->Send( "FETCh:SEGMent1:OFDM:FLATness:CHECk?" );
                if( ! Tester->mp5k_list.at( handle )->IsWaitTillReadToken( FLATNESS , "0," , 2) )
                {
                    return MP5K_TXEX_READ_FLATNESS_ERROR;
                }
				SubString( FLATNESS , 2 );
				SubString( MARGIN , 2 );
				SubString( FREQ , 2 );


				int flatness = atoi( FLATNESS.c_str() );
                if (flatness == 0)
                {
                    txmeasext.OFDM.Flatness = true;
                }
                else
                {
                    txmeasext.OFDM.Flatness = false;
                }
               
                tempMargin.clear();
                tempFreq.clear();
/*
				boost::split( tempMargin, std::string(MARGIN) , boost::is_any_of(","));
				boost::split( tempFreq, std::string(FREQ) , boost::is_any_of(","));
*/
                boost::split( tempMargin,  (MARGIN) , boost::is_any_of(","));  // allen
                boost::split( tempFreq,  (FREQ) , boost::is_any_of(","));
               //if( Tester->gloTXConfig.Standard.OFDM.Modulation.Bandwidth == _80M )
               //{
               // txmeasext.OFDM.Lower3.FreqOffset    = 0.00;
               // txmeasext.OFDM.Lower3.Margin        = 0.00;
               // txmeasext.OFDM.Lower4.FreqOffset    = 0.00;
               // txmeasext.OFDM.Lower4.Margin        = 0.00;
               // txmeasext.OFDM.Upper3.FreqOffset    = 0.00;
               // txmeasext.OFDM.Upper3.Margin        = 0.00;
               // txmeasext.OFDM.Upper4.FreqOffset    = 0.00;
               // txmeasext.OFDM.Upper4.Margin        = 0.00;
               //}
               //else
               //{
					txmeasext.OFDM.Lower3.FreqOffset    = atof(tempFreq.at(6).c_str())/ 10000000.00;
                    txmeasext.OFDM.Lower3.Margin        = atof(tempMargin.at(6).c_str()); 
                    txmeasext.OFDM.Lower4.FreqOffset    = atof(tempFreq.at(7).c_str())/ 10000000.00;
                    txmeasext.OFDM.Lower4.Margin        = atof(tempMargin.at(7).c_str());
                    txmeasext.OFDM.Upper3.FreqOffset    = atof(tempFreq.at(1).c_str())/ 10000000.00;
                    txmeasext.OFDM.Upper3.Margin        = atof(tempMargin.at(1).c_str());
                    txmeasext.OFDM.Upper4.FreqOffset    = atof(tempFreq.at(0).c_str())/ 10000000.00;
                    txmeasext.OFDM.Upper4.Margin        = atof(tempMargin.at(0).c_str());
                //}
                txmeasext.OFDM.Lower1.FreqOffset    = atof(tempFreq.at(4).c_str() )/ 10000000.00;
                txmeasext.OFDM.Lower1.Margin        = atof(tempMargin.at(4).c_str() );
                txmeasext.OFDM.Lower2.FreqOffset    = atof(tempFreq.at(5).c_str() )/ 10000000.00;
                txmeasext.OFDM.Lower2.Margin        = atof(tempMargin.at(5).c_str() );
                txmeasext.OFDM.Upper1.FreqOffset    = atof(tempFreq.at(3).c_str() )/ 10000000.00;
                txmeasext.OFDM.Upper1.Margin        = atof(tempMargin.at(3).c_str() );
                txmeasext.OFDM.Upper2.FreqOffset    = atof(tempFreq.at(2).c_str() )/ 10000000.00;
                txmeasext.OFDM.Upper2.Margin        = atof(tempMargin.at(2).c_str() );

                if( Tester->gloTXConfig.Standard.type == _802_11AC && Tester->gloTXConfig.Standard.OFDM.Modulation.Bandwidth == _80M )
				{
                    Tester->mp5k_list.at( handle )->Send( "CONFigure:OFDM:SPECtrum:WIDE OFF" );
                    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
					{
						return MP5K_TX_SET_WIDE_ERROR;
					}
				}


            }
            else
            {
                Tester->mp5k_list.at( handle )->Send( "FETCh:SEGMent1:DSSS:RAMP:ON:CHECK?");
                if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( RAMP_ON , "\n" ) )
                {
                    return MP5K_TXEX_READ_RAMP_ON_ERROR;
                }
                Tester->mp5k_list.at( handle )->Send( "FETCh:SEGMent1:DSSS:RAMP:OFF:CHECK?");
                if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( RAMP_OFF , "\n" ) )
                {
                    return MP5K_TXEX_READ_RAMP_OFF_ERROR;
                }
				if( !SubString( RAMP_ON, 2 ) ||	!SubString( RAMP_OFF, 2 ) || !SubString( MARGIN, 2 ) ||! SubString( FREQ, 2 ) )
				{
					return MP5K_TXEX_READ_RAMP_OFF_ERROR;
				}


				int rampOff = atoi( RAMP_OFF.c_str() );
                if (rampOff == 0)
                {
                    txmeasext.DSSS.RampOff = true;
                }
                else
                {
                    txmeasext.DSSS.RampOff = false;
                }
                int rampOn = atoi( RAMP_ON.c_str() );
                if (rampOn == 0)
                {
                    txmeasext.DSSS.RampOn = true;
                }
                else
                {
                    txmeasext.DSSS.RampOn = false;
                }


                tempMargin.clear();
                tempFreq.clear();
/*
				boost::split( tempMargin, std::string(MARGIN) , boost::is_any_of(","));
				boost::split( tempFreq, std::string(FREQ) , boost::is_any_of(","));
*/
                boost::split( tempMargin, (MARGIN) , boost::is_any_of(","));
                boost::split( tempFreq, (FREQ) , boost::is_any_of(","));
				txmeasext.DSSS.Lower1.FreqOffset    = atof(tempFreq.at(2).c_str()) / 10000000.00;
                txmeasext.DSSS.Lower1.Margin        = atof(tempMargin.at(2).c_str());
                txmeasext.DSSS.Lower2.FreqOffset    = atof(tempFreq.at(3).c_str())/ 10000000.00;
                txmeasext.DSSS.Lower2.Margin        = atof(tempMargin.at(3).c_str());
                txmeasext.DSSS.Upper1.FreqOffset    = atof(tempFreq.at(1).c_str())/ 10000000.00;
                txmeasext.DSSS.Upper1.Margin        = atof(tempMargin.at(1).c_str());
                txmeasext.DSSS.Upper2.FreqOffset    = atof(tempFreq.at(0).c_str())/ 10000000.00;
                txmeasext.DSSS.Upper2.Margin        = atof(tempMargin.at(0).c_str());
            }
        }
    }
    return MP5K_OK;
}

//BlueTooth

UINT mp5k_BT_tx_setting (const UINT& handle, const MP5K_BT_TxTestConfig& Setting)
{
	if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_tx_setting:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
    Tester->gloBtTXConfig = Setting;
	
	UINT OtherPort[3], cnt = 0;
	for(int port = 1 ;port < 5; port++)
	{
        if(port !=  Tester->gloBtTXConfig.UseRFPort)
		{
			OtherPort[cnt] = port;
			cnt++;
		}
	}
 	if( isFourPort )
	{
		sprintf_s( strBuffer ,
            "BT;:ROUTe:PORT:RESource RF%d,VSA;:ROUTe:PORT:RESource RF%d,OFF;:ROUTe:PORT:RESource RF%d,OFF;:ROUTe:PORT:RESource RF%d,OFF;:INPut:FREQuency %dMHZ;:INPut:LEVel:REFerence %.2f;:INPut:CAPTure:TIME %.6f" ,
            Tester->gloBtTXConfig.UseRFPort ,
			OtherPort[0] ,
			OtherPort[1] ,
			OtherPort[2] ,
            Tester->gloBtTXConfig.FreqMHz ,
            Tester->gloBtTXConfig.RefLeveldBm ,
            Tester->gloBtTXConfig.CaptureLength
            );
	}
	else
	{
		sprintf_s( strBuffer ,
            "BT;:ROUTe:PORT:RESource RF%d,VSA;:INPut:FREQuency %dMHZ;:INPut:LEVel:REFerence %.2f;:INPut:CAPTure:TIME %.6f" ,
            Tester->gloBtTXConfig.UseRFPort ,
            Tester->gloBtTXConfig.FreqMHz ,
            Tester->gloBtTXConfig.RefLeveldBm ,
            Tester->gloBtTXConfig.CaptureLength
            );
	}

    Tester->mp5k_list.at( handle )->Send( strBuffer );
    
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_STANDARD_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_USEPORT_ERROR;
    }
	
	if( isFourPort )
	{
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
		    return MP5K_USEPORT_ERROR;
		}
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
		    return MP5K_USEPORT_ERROR;
		}
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
		    return MP5K_USEPORT_ERROR;
		}
	}


    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_TX_FREQUENCY_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_TX_REF_LEVEL_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_TX_CAPTURE_LENGTH_ERROR;
    }


    Tester->mp5k_list.at( handle )->Send( "CONFigure:STANdard?" );
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_STANDARD_ERROR;
    }
	bool isSameStandard = boost::equals("BT\n", ackBuffer );
	
	if( !isSameStandard )
	{
        Tester->mp5k_list.at( handle )->Send( "BT" );
        if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
		{
			return MP5K_STANDARD_ERROR;
		}
	}




    Tester->mp5k_list.at( handle )->Send( "INPut:STARt" );
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_MEAS_TIME_OUT;
    }
    // write to log
#if MP5KLOG
    sprintf_s( strBuffer ,
               "mp5k_BT_tx_setting:[Handle = %d,Type = %d,Port = %d,Freq(MHz)=%d,Ref level(dBm)= %.2f,Capture length(ms)=%.3f]" ,
               handle ,
               Tester->gloBtTXConfig.Type,
               Tester->gloBtTXConfig.UseRFPort,
               Tester->gloBtTXConfig.FreqMHz,
               Tester->gloBtTXConfig.RefLeveldBm,
               Tester->gloBtTXConfig.CaptureLength * 1000 // ms
             );
    Mp5kLog.WriteLine( strBuffer );
#endif
	isBT_TX_Sync = false;
    return MP5K_OK;
}

UINT mp5k_BT_tx_meas(const UINT& handle, MP5K_BT_TxTestMeas& Bt_Tx_Meas)
{
	if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_BT_tx_meas:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
    // Mode
    char standard[ BUFFER_LENGTH ];

	sprintf_s( standard , "BT" );
	sprintf_s( strBuffer , "CALCulate:TXQuality:ALL:SYNC;:FETCh:SEGMent1:BT:Power?" );

    std::string Sync;
    std::string Power;
	std::string Return;
    
    Tester->mp5k_list.at( handle )->Send( strBuffer );
    
    bool IsSync = Tester->mp5k_list.at( handle )->IsWaitTillReadToken( Sync , "\n" );
    bool IsPower = Tester->mp5k_list.at( handle )->IsWaitTillReadToken( Power , "\n" );
    
	if ( IsSync )
    {
        /*--------------------------------------------------------------------------------------------------------------------------------*/
        // Check Sync
        int retSync = atoi( Sync.c_str() );
        if ( retSync == -200 )
        {
#if MP5KLOG
            sprintf_s( strBuffer , "mp5k_BT_tx_meas:[Handle = %d,Sync failed]" , handle );
            Mp5kLog.WriteLine( strBuffer );
#endif
			std::string CaptureCmd = "CAPTure:STORe BtSync failed_" + boost::lexical_cast<std::string>( gloMyID ) + ".wfm";
            Tester->mp5k_list.at( handle )->Send( CaptureCmd );

            Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" );
            return MP5K_SYNC_TIMEOUT;
            /*--------------------------------------------------------------------------------------------------------------------------------*/
        }
        else
        {
            /*--------------------------------------------------------------------------------------------------------------------------------*/
            // Get Power, Phase Error, Frequency Error, Symbol Clock Error, Lo Leakage, MASK, EVM
            if ( !IsPower )
            {
#if MP5KLOG
                sprintf_s( strBuffer , "mp5k_BT_tx_meas:[Handle = %d,Power measurement timeout]" , handle );
                Mp5kLog.WriteLine( strBuffer );
#endif
                return MP5K_MEAS_TIME_OUT;
            }
            int retPower = atoi( Power.c_str() );
            if ( retPower == -200)
            {
				std::string CaptureCmd = "CAPTure:STORe BtPower failed_" + boost::lexical_cast<std::string>( gloMyID ) + ".wfm";
                Tester->mp5k_list.at( handle )->Send( CaptureCmd );

                Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" );
#if MP5KLOG
                sprintf_s( strBuffer ,"mp5k_BT_tx_meas:[Handle = %d,Power failed]",handle);
                Mp5kLog.WriteLine( strBuffer );
#endif
                return MP5K_MEAS_FAILED;
            }
			SubString(Power, 2);
            
			Bt_Tx_Meas.BT_TargetPower = atof(Power.c_str());
          

            switch( Tester->gloBtTXConfig.Type )
			{
			case DM1:
			case DH1:
			case DM3:
			case DH3:
			case DM5:
			case DH5:
				sprintf_s( strBuffer , "FETCh:SEGMent1:BT:CLASsic?" );
				break;
			case _2DH1:
			case _2DH3:
			case _2DH5:
			case _3DH1:
			case _3DH3:
			case _3DH5:
				sprintf_s( strBuffer , "FETCh:SEGMent1:BT:EDRate?" );
				break;
			case LE:
				sprintf_s( strBuffer , "FETCh:SEGMent1:BT:LENergy?" );
				break;
			default:
				return MP5K_MEAS_FAILED;
			}
			
            Tester->mp5k_list.at( handle )->Send( strBuffer );
            bool IsReturn = Tester->mp5k_list.at( handle )->IsWaitTillReadToken( Return , "\n" );
			if( !IsReturn )
			{
#if MP5KLOG
                sprintf_s( strBuffer , "mp5k_BT_tx_meas:[Handle = %d,Power measurement timeout]" , handle );
                Mp5kLog.WriteLine( strBuffer );
#endif
                return MP5K_MEAS_TIME_OUT;
			}

			if( -200 == atoi( Return.c_str() ) )
			{
#if MP5KLOG
                sprintf_s( strBuffer , "mp5k_BT_tx_meas:[Handle = %d,Power measurement timeout]" , handle );
                Mp5kLog.WriteLine( strBuffer );
#endif
                return MP5K_MEAS_TIME_OUT;
			}
			std::vector<std::string> returnData;
			SubString(Return, 2);
//			boost::split( returnData, std::string(Return) , boost::is_any_of(",")); //allen
            boost::split( returnData,  (Return) , boost::is_any_of(","));

			Bt_Tx_Meas.BT_InitFreq_Error = atof(returnData[0].c_str());
            switch( Tester->gloBtTXConfig.Type )
			{
			case DM1:
			case DH1:
			case DM3:
			case DH3:
			case DM5:
			case DH5:
				Bt_Tx_Meas.BT_TX_ReturnValue.BT_BDR.BW_20db = atof(returnData[3].c_str());
				Bt_Tx_Meas.BT_TX_ReturnValue.BT_BDR.DeltaF1Avg = atof(returnData[10].c_str());
				Bt_Tx_Meas.BT_TX_ReturnValue.BT_BDR.DeltaF2Avg = atof(returnData[11].c_str());
				Bt_Tx_Meas.BT_TX_ReturnValue.BT_BDR.DeltaF2Max = atof(returnData[12].c_str());
				Bt_Tx_Meas.BT_TX_ReturnValue.BT_BDR.FreqDrift = atof(returnData[4].c_str());
				break;
			case _2DH1:
			case _2DH3:
			case _2DH5:
			case _3DH1:
			case _3DH3:
			case _3DH5:
				Bt_Tx_Meas.BT_TX_ReturnValue.BT_EDR.BT_DEVM = atof(returnData[6].c_str());
				Bt_Tx_Meas.BT_TX_ReturnValue.BT_EDR.BT_DEVMPeak = atof(returnData[7].c_str());
				break;
			case LE:
				Bt_Tx_Meas.BT_TX_ReturnValue.BT_LE.DeltaF1Avg = atof(returnData[1].c_str());
				Bt_Tx_Meas.BT_TX_ReturnValue.BT_LE.DeltaF2Avg = atof(returnData[2].c_str());
				Bt_Tx_Meas.BT_TX_ReturnValue.BT_LE.DeltaF2Max = atof(returnData[3].c_str());
				break;
			default:
				return MP5K_MEAS_FAILED;
			}
		
#if MP5KLOG
            sprintf_s( strBuffer ,
                       "mp5k_BT_tx_meas: Handle=%d,TX Power(dBm)=%.2f,InitFreq_error=%.2f" ,
                       handle ,
                       Bt_Tx_Meas.BT_TargetPower ,
                       Bt_Tx_Meas.BT_InitFreq_Error
                     );
            Mp5kLog.WriteLine( strBuffer );
#endif
			isBT_TX_Sync = true;
            return MP5K_OK;
            /*--------------------------------------------------------------------------------------------------------------------------------*/
        }
    }
    else
    {
        /*--------------------------------------------------------------------------------------------------------------------------------*/
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_BT_tx_meas:[Handle = %d, Sync timeout]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_MEAS_TIME_OUT;
        /*--------------------------------------------------------------------------------------------------------------------------------*/
    }
}

UINT mp5k_BT_tx_meas_extend(const UINT& handle, MP5K_BT_TxTestMeasExtend& Bt_Tx_MeasExt)
{
	if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_BT_tx_meas_extend:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }

 
	std::string Sync;
	std::string ACP_Return;
	std::string ACP_Check_Return;
	bool IsSync = true;
	if( !isBT_TX_Sync )
	{
        Tester->mp5k_list.at( handle )->Send( "CALCulate:TXQuality:ALL:SYNC" );
        IsSync = Tester->mp5k_list.at( handle )->IsWaitTillReadToken( Sync , "\n" );
	}
	
	if ( IsSync )
    {
        /*--------------------------------------------------------------------------------------------------------------------------------*/
        // Check Sync
		int retSync = atoi( Sync.c_str() );
        if ( retSync == -200 )
        {
#if MP5KLOG
            sprintf_s( strBuffer , "mp5k_BT_tx_meas_extend:[Handle = %d,Sync failed]" , handle );
            Mp5kLog.WriteLine( strBuffer );
#endif
			std::string CaptureCmd = "CAPTure:STORe BtSync failed_" + boost::lexical_cast<std::string>( gloMyID ) + ".wfm";
            Tester->mp5k_list.at( handle )->Send( CaptureCmd );
            Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" );
            return MP5K_SYNC_TIMEOUT;
            /*--------------------------------------------------------------------------------------------------------------------------------*/
        }
        else
        {
            Tester->mp5k_list.at( handle )->Send( "FETCh:SEGMent1:BT:ACPower?;ACPower:CHECk?" );
            bool IsACP_Return = Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ACP_Return , "\n" );
            bool IsACP_Check_Return = Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ACP_Check_Return , "\n" );
			if( !IsACP_Return )
			{
#if MP5KLOG
                sprintf_s( strBuffer , "mp5k_BT_tx_meas_extend:[Handle = %d,Power measurement timeout]" , handle );
                Mp5kLog.WriteLine( strBuffer );
#endif
                return MP5K_MEAS_TIME_OUT;
			}

			if( -200 == atoi( ACP_Return.c_str() ) )
			{
#if MP5KLOG
                sprintf_s( strBuffer , "mp5k_BT_tx_meas_extend:[Handle = %d,Power measurement timeout]" , handle );
                Mp5kLog.WriteLine( strBuffer );
#endif
                return MP5K_MEAS_TIME_OUT;
			}
			
			if( SubString( ACP_Return, 2 ) )
			{
				std::vector<std::string> returnData;
//				boost::split( returnData, std::string(ACP_Return) , boost::is_any_of(","));  //allen
                boost::split( returnData,  (ACP_Return) , boost::is_any_of(","));

                if( returnData.size() < 11 )
				{
					return 0;
				}
				Bt_Tx_MeasExt.ACP_Data.Offset_L1 = atof(returnData.at(0).c_str());
				Bt_Tx_MeasExt.ACP_Data.Offset_L2 = atof(returnData.at(1).c_str());
				Bt_Tx_MeasExt.ACP_Data.Offset_L3 = atof(returnData.at(2).c_str());
				Bt_Tx_MeasExt.ACP_Data.Offset_L4 = atof(returnData.at(3).c_str());
				Bt_Tx_MeasExt.ACP_Data.Offset_L5 = atof(returnData.at(4).c_str());
				Bt_Tx_MeasExt.ACP_Data.Offset_0 = atof(returnData.at(5).c_str());
				Bt_Tx_MeasExt.ACP_Data.Offset_R1 = atof(returnData.at(6).c_str());
				Bt_Tx_MeasExt.ACP_Data.Offset_R2 = atof(returnData.at(7).c_str());
				Bt_Tx_MeasExt.ACP_Data.Offset_R3 = atof(returnData.at(8).c_str());
				Bt_Tx_MeasExt.ACP_Data.Offset_R4 = atof(returnData.at(9).c_str());
				Bt_Tx_MeasExt.ACP_Data.Offset_R5 = atof(returnData.at(10).c_str());
			}
			else
			{
#if MP5KLOG
                sprintf_s( strBuffer , "mp5k_BT_tx_meas_extend:[Handle = %d,SubString Error]" , handle );
                Mp5kLog.WriteLine( strBuffer );
#endif
				return MP5K_MEAS_FAILED;
			}
			

			if( SubString( ACP_Check_Return, 2 ) )
			{
				Bt_Tx_MeasExt.ACP_Check = 0 == atoi(ACP_Check_Return.c_str() );
			}
			else
			{
#if MP5KLOG
                sprintf_s( strBuffer , "mp5k_BT_tx_meas_extend:[Handle = %d,SubString Error]" , handle );
                Mp5kLog.WriteLine( strBuffer );
#endif
				return MP5K_MEAS_FAILED;
			}
            return MP5K_OK;
            /*--------------------------------------------------------------------------------------------------------------------------------*/
        }
    }
    else
    {
        /*--------------------------------------------------------------------------------------------------------------------------------*/
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_BT_tx_meas_extend:[Handle = %d, Sync timeout]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_MEAS_TIME_OUT;
        /*--------------------------------------------------------------------------------------------------------------------------------*/
    }
}

UINT mp5k_BT_rx_setting(const UINT& handle, const MP5K_BT_RxTestConfig& Setting)
{
	if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_BT_rx_setting:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
    Tester->gloBtRXConfig = Setting;
    UINT uTime = 0;	

	char	packetType[ BUFFER_LENGTH ];


    switch( Tester->gloBtRXConfig.Type )
    {
	case DH1:		sprintf_s( packetType , "DH1" );		break;
	case DH3:		sprintf_s( packetType , "DH3" );		break;
	case DH5:		sprintf_s( packetType , "DH5" );		break;
	case _2DH1:		sprintf_s( packetType , "2DH1" );		break;
	case _2DH3:		sprintf_s( packetType , "2DH3" );		break;
	case _2DH5:		sprintf_s( packetType , "2DH5" );		break;
	case _3DH1:		sprintf_s( packetType , "3DH1" );		break;
	case _3DH3:		sprintf_s( packetType , "3DH3" );		break;
	case _3DH5:		sprintf_s( packetType , "3DH5" );		break;
	case LE:		sprintf_s( packetType , "LE" );			break;
	default:		return MP5K_MEAS_FAILED;
	}
	

	/* allen 20170623
    sprintf_s( strBuffer ,
               "BT;:ROUTe:PORT:RESource RF%d,VSG;:OUTPut:FREQuency %dMHz;:OUTPut:POWer:LEVel %.2f;:OUTPut:LOOP:COUNt %d;:OUTPut:WAVeform:LOAD \\MP\\BT\\BT_%s_PRBS9.mp5000vsg" ,
               Tester->gloBtRXConfig.UseRFPort ,
               Tester->gloBtRXConfig.FreqMHz ,
               Tester->gloBtRXConfig.OutPowerdBm,
               Tester->gloBtRXConfig.BitCount ,
               packetType
             );
	*/
	  sprintf_s( strBuffer ,
               "BT;:ROUTe:PORT:RESource RF%d,VSG;:OUTPut:FREQuency %dMHz;:OUTPut:POWer:LEVel %.2f;:OUTPut:LOOP:COUNt %d" ,
               Tester->gloBtRXConfig.UseRFPort ,
               Tester->gloBtRXConfig.FreqMHz ,
               Tester->gloBtRXConfig.OutPowerdBm,
               Tester->gloBtRXConfig.BitCount ,
               packetType
             );
    Tester->mp5k_list.at( handle )->Send( strBuffer );
    
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_STANDARD_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_USEPORT_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_RX_FREQUENCY_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_RX_OUTPUT_POWER_ERROR;
    }
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_RX_PACKET_COUNT_ERROR;
    }
	/* 20170623
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
    {
        return MP5K_RX_WAVEFORM_ERROR;
    }
	*/
    sprintf_s( strBuffer ,"CONFigure:WAVE:GAP %f",  Tester->gloBtRXConfig.GapTime);
    Tester->mp5k_list.at( handle )->Send( strBuffer );
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) )
	{
		return MP5K_RX_GAP_ERROR;
	}

    // write to log
#if MP5KLOG
    sprintf_s( strBuffer ,
               "mp5k_BT_rx_setting:[Handle = %d,Output freq(Mhz) = %d,Output level(dBm) = %.2f" ,
               handle ,              
               Setting.FreqMHz ,
               Setting.OutPowerdBm ,
			   Setting.BitCount
             );
    Mp5kLog.WriteLine( strBuffer );
#endif
    return MP5K_OK;
}

UINT mp5k_BT_rx_start(const UINT& handle)
{
	if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_BT_rx_start:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }

    Tester->mp5k_list.at( handle )->Send( "OUTPut:STARt" );
	// Check rx timeout
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" , BT_rx_time_out ))
	{
		//mp5k tester refresh or clear
        Tester->mp5k_list.at( handle )->Close();
        Tester->mp5k_list.at( handle )->Open( Tester->mp5k_list.at( handle )->ip , 5499 , 2 );
		// write to log
#if MP5KLOG
		sprintf_s( strBuffer , "mp5k_BT_rx_start:[Handle = %d,Timeout]" , handle );
		Mp5kLog.WriteLine( strBuffer );
#endif
    return MP5K_RX_START_TIMEOUT;
	}
    // write to log
#if MP5KLOG
    sprintf_s( strBuffer , "mp5k_BT_rx_start:[Handle = %d,Started]" , handle );
    Mp5kLog.WriteLine( strBuffer );
#endif
    return MP5K_OK;
}


#if GEMTEKS

UINT mp5k_VSA_AGC(const UINT& handle)
{
	if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_VSA_agc:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }

    Tester->mp5k_list.at( handle )->Send( "INPut:LEVel:REFerence:AUTO" );
	
    if ( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( strBuffer , "\n" ) )
    {
        return MP5K_AGC_ERROR;
    }
	 return MP5K_OK;
}

UINT mp5k_GetSpectrumPower( const UINT& handle, double& StartFreq, double& OffsetFreq ,std::vector<float>& data )
{
	/*
	if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_GetSpectrumPower:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
	
    
    Tester->mp5k_list.at( handle )->Send( "CALCulate:TXQuality:ALL:SYNC;" );
    Tester->mp5k_list.at( handle )->IsWaitTillReadToken( strBuffer , "\n" );
    int retSync = atoi( strBuffer );
	if ( retSync == -200 )
    {
#if MP5KLOG
        sprintf_s( strBuffer ,"mp5k_GetSpectrumPower:[Handle = %d,Sync failed]" ,handle);
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_SYNC_TIMEOUT;
    }

    if( !Tester->gloTXConfig.Standard.IsGap )
	{
		sprintf_s( strBuffer , "FETCh:SPECtrum:GPRF:DATA?" );
	}
    else if( Tester->gloTXConfig.Standard.type == _802_11B )
	{
		sprintf_s( strBuffer , "FETCh:SPECtrum:DSSS:DATA?" );
	}
	else
	{
		sprintf_s( strBuffer , "FETCh:SPECtrum:OFDM:DATA?" );
	}
	
    Tester->mp5k_list.at( handle )->Send( strBuffer );
	char* ackBuffer = new char[10240];

    if ( Tester->mp5k_list.at( handle )->IsWaitTillReadTokenForSpectrum( ackBuffer ,10240, "\n"  ))
	{
		data.clear();

		std::string ackData( ackBuffer );
		std::vector<std::string> ackVecData;
		boost::split( ackVecData, ackData , boost::is_any_of(","));
		std::string Error = ackVecData.at(0);
		
		StartFreq = boost::lexical_cast<double>(ackVecData.at(1).c_str());
		OffsetFreq = boost::lexical_cast<double>(ackVecData.at(2).c_str());
		
		char numDigitalChar = ackVecData.at(3).at(1);
		int numDigital = numDigitalChar - '0';
		int numByte = atoi(ackVecData.at(3).substr(2,numDigital).c_str() );
		int DataSize = numByte / sizeof(float);

		const float* pf=reinterpret_cast<const float*>( ackVecData.at(3).data() );

		data.assign(pf, pf+numByte);
	}
	else
	{
		return 999;
	}
	*/
	return MP5K_OK;
}

UINT mp5k_VSA_SaveWaveform(const UINT& handle, const std::string& filename )
{
	if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_VSA_SaveWaveform:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
	sprintf_s(strBuffer, "CALCulate:TXQuality:ALL:SYNC;:CAPTure:STORe %s", filename.c_str() );
    Tester->mp5k_list.at( handle )->Send( strBuffer );
    bool IsSync = Tester->mp5k_list.at( handle )->IsWaitTillReadToken( strBuffer , "\n" );
    bool IsCaP = Tester->mp5k_list.at( handle )->IsWaitTillReadToken( strBuffer , "\n" ) ;
		
	if( !IsSync || !IsCaP )
	{
		return 999;
	}

	return MP5K_OK;
}

UINT mp5k_VSA_LoadWaveform(const UINT& handle, const std::string& filename )
{
	if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_VSA_LoadWaveform:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
	
	sprintf_s(strBuffer, "INPut:WAVeform:LOAD %s", filename.c_str() );
    Tester->mp5k_list.at( handle )->Send( strBuffer );
    bool isLoad =  Tester->mp5k_list.at( handle )->IsWaitTillReadToken( strBuffer , "\n" );
	if( !isLoad )
	{
		return 999;
	}

	return MP5K_OK;
}

UINT mp5k_RxStartMode( const MP5K_RX_Start_Mode& Async )
{
	WIFI_RX_ASYNC_MODE = Async; 
	switch( Async )
	{
	case RX_START_SYNC:		WIFI_RX_ASYNC_MODE = Async;		break;
	case RX_START_ASYNC:	WIFI_RX_ASYNC_MODE = Async;		break;
	default:		WIFI_RX_ASYNC_MODE = RX_START_SYNC;		break;
	}

#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_RxStartMode:[ %u ]" , WIFI_RX_ASYNC_MODE );
        Mp5kLog.WriteLine( strBuffer );
#endif
    return MP5K_OK;
} 

UINT mp5k_waveform_Upload(const UINT& handle, const std::string& filename)
{
/*	if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_Wavefrom_Upload:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
	std::string UserPath = "D:\\Adivic\\MP5000 Tester\\mpvsg\\USER\\Port" + boost::lexical_cast<std::string>(gloMyID) + "\\";
	std::string pathCmd = "MMEMory:MDirectory " + UserPath + "";
	
    Tester->mp5k_list.at( handle )->Send( pathCmd );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( strBuffer , "\n" ) )
	{
		return 999;
	}

	std::string onlyfilename;
	FILE* file = fopen( filename.c_str() , "rb" );

	if( file == nullptr )
	{
		return 999;
	}
	boost::filesystem::path p(filename.c_str() );
	onlyfilename = p.filename().string();

	fseek (file , 0 , SEEK_END);
	long  lSize = ftell(file);
	rewind (file);

	unsigned char* OriginalBuffer = new unsigned char[ lSize + 1];
	char* buffer2 = new char[ lSize + 512];
	char* ASCII_Buffer = new char[ lSize  * 2];
	
	if (fread(OriginalBuffer,1, lSize,file) != lSize ) 	
	{
		SAFE_DELETE( OriginalBuffer );
		SAFE_DELETE( buffer2 );
		SAFE_DELETE( ASCII_Buffer );
		return 999;
	}

	char ASCII_Temp[3] = {0};
	int NewSize = lSize * 2;
	for( int i= 0 ; i < NewSize; i+= 2)
	{
		sprintf(ASCII_Temp,"%02x",*OriginalBuffer);
		ASCII_Buffer[ i ]	= ASCII_Temp[0];
		ASCII_Buffer[ i+1 ]	= ASCII_Temp[1];
		++OriginalBuffer;
	}


	int LastDataSize = NewSize % 1024;
	int TotalCount = (NewSize / 1024.0) ;
	if( LastDataSize != 0 )
	{
		++TotalCount;
	}
	bool IsSync = false;
	
	for( int Count = 0 ; Count < TotalCount ; ++Count )
	{
		std::stringstream SCPI_Data;
		unsigned temp;
		unsigned dataSize;
		unsigned digitNumber = 0;
		if( Count == TotalCount && LastDataSize != 0)
		{
			dataSize = temp = LastDataSize;
		}
		else
		{
			dataSize = temp = 1024;
		}
		
		do
		{
			temp /= 10;
			++digitNumber;
		} while (temp > 0);


		SCPI_Data << "MMEMory:DATA "<< UserPath << p.filename().string() << ","<< TotalCount << "," << Count + 1 << ",#" << digitNumber << dataSize;

		memcpy( buffer2 , SCPI_Data.str().c_str() , SCPI_Data.str().length() );
		memmove( buffer2 + SCPI_Data.str().length() , ASCII_Buffer + ( Count * dataSize ), dataSize  );
		memmove( buffer2 + SCPI_Data.str().length() + dataSize , "" , 2 );
		size_t totoalSize = SCPI_Data.str().length()  + dataSize + 2 ;
	
        Tester->mp5k_list.at( handle )->Send( buffer2 , totoalSize);
        IsSync = Tester->mp5k_list.at( handle )->IsWaitTillReadToken( strBuffer , "\n" );
	}


	if( IsSync )
	{
		WaveformQueue.insert( onlyfilename );
	}
	
	fclose (file);
	for( int i= 0 ; i < NewSize; i+= 2)
	{
		--OriginalBuffer;
	}
	SAFE_DELETE( OriginalBuffer );
	SAFE_DELETE( buffer2 );
	SAFE_DELETE( ASCII_Buffer );
	*/
	return MP5K_OK;
	
}

UINT mp5k_Delete_File(const UINT& handle, const std::string& filename)
{
	if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
        // write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_Delete_File:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
		return MP5K_HANDLE_ERROR;
	}

	sprintf_s( strBuffer , "MMEMory:DELete %s" , filename.c_str()  );
    Tester->mp5k_list.at( handle )->Send( strBuffer );
	// Check rx timeout
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( strBuffer , "\n" , rx_time_out ))
	{
        return MP5K_HANDLE_ERROR;
    }
	return MP5K_OK;
}

#endif

#if GPSAPI
// GPS 
UINT mp5k_GPS_init( void )
{
	if( !isGPSInit )
	{
        //GPSTester = new AdivicTask< AdivicTester >( AdivicTester() );
        GPSTester = new   AdivicTester();
		if ( GPSTester == nullptr  )
		{
			// write to log
#if MP5KLOG
			sprintf_s( strBuffer , "mp5k_GPS_init:[Init failed]" );
			Mp5kLog.WriteLine( strBuffer );
#endif
			return MP5K_TESTER_NULL;
		}
		ADIVIC_Sync_TelnetClient* tmp = nullptr;		
        GPSTester->mp5k_list.push_back( tmp ); // no use
		// default multi-dut variable
        GPSTester->MultiDUTEnable = false;




		// write to log
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_GPS_init:[MultiDUT = %d]", GPSTester->MultiDUTEnable );
		Mp5kLog.WriteLine( strBuffer );
#endif
		gloMyID = 1;
		gloNextID = 1;
		// write to log
#if MP5KLOG
		sprintf_s( strBuffer , "mp5k_GPS_init:[MyID = %d,NextID = %d]", gloMyID , gloNextID );
		Mp5kLog.WriteLine( strBuffer );
#endif
		isGPSInit = true;
	}
	return MP5K_OK;
}

UINT mp5k_GPS_add( const char* ip , UINT&  handle)
{
	ADIVIC_Sync_TelnetClient* tmp = nullptr;

	//tmp = new AdivicTClient("GPS MainLog.txt" );
	tmp = new ADIVIC_Sync_TelnetClient( "GPS_LOG" );
	if ( tmp == nullptr )
	{
		// write to log
#if MP5KLOG
		sprintf_s( strBuffer , "mp5k_GPS_add:[Client create failed]" );
		Mp5kLog.WriteLine( strBuffer );
#endif
		return MP5K_CLIENT_FAILED;
	}

	if( 0 == strcmp(preIP,ip) )
	{
        if( GPSTester->mp5k_list.at( handle )->IsConnected() )
		{
			return ::MP5K_OK;
		}
        GPSTester->mp5k_list.clear();
		ADIVIC_Sync_TelnetClient* temp = nullptr;
        GPSTester->mp5k_list.push_back( temp ); // no use
	}
	int id = 0;
	if ( !tmp->Open( ip , 6341 , 2 ) )    // 2s, timeout, disconnect
	{
		id = 0;
		tmp->Close();
		delete tmp;
// write to log
#if MP5KLOG
		sprintf_s( strBuffer , "mp5k_GPS_add:[Client connect timeout]" );
		Mp5kLog.WriteLine( strBuffer );
#endif
		return MP5K_CONNECT_TIMEOUT; // id
	}
	else
	{
        id = GPSTester->mp5k_list.size();
        GPSTester->mp5k_list.push_back( tmp );
		handle = id;
		strcpy_s(preIP,ip);
	}
// write to log
#if MP5KLOG
	sprintf_s(strBuffer, "mp5k_GPS_add:[IP = %s,Handle = %d]", ip, id);
	Mp5kLog.WriteLine( strBuffer );
#endif    

	return MP5K_OK;
}

UINT mp5k_GPS_delete( const UINT& handle )
{
	if ( GPSTester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle != NULL )
    {
        // notice: Watchdog close
        //GPSTester->stop();  // allen
        //Sleep(10);
        usleep(10 *1000);
        if ( GPSTester->mp5k_list.at( handle ) )
        {
#if MP5KLOG
			sprintf_s(strBuffer, "mp5k_GPS_delete:[Start Remove handle = %d]", handle);
			Mp5kLog.WriteLine( strBuffer );
#endif
            GPSTester->mp5k_list.at( handle )->Close();
            // Sleep(10);
            usleep(10 *1000);
            delete GPSTester->mp5k_list.at( handle );
        }
        // write to log
#if MP5KLOG
        sprintf_s(strBuffer, "mp5k_GPS_delete:[Remove handle = %d]", handle);
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_OK;
    }
    else
    {
        // write to log
#if MP5KLOG
        sprintf_s(strBuffer, "mp5k_GPS_delete:[Handle error]", handle);
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
    }
}

UINT mp5k_GPS_close( const UINT& handle )
{
	if ( GPSTester )
    {
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_GPS_close:[Success,WatchDog Closed]" );
        Mp5kLog.WriteLine( strBuffer );
#endif
       // Sleep( 10 );
        usleep(10*1000);
        delete GPSTester;
		GPSTester = nullptr;
    }
    // write to log
    return MP5K_OK;
}

UINT mp5k_GPS_SingleCh_Setting( const UINT& handle ,const GPS_SingleCh_Config& Setting)
{
	if ( GPSTester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle != NULL )
    {
		sprintf_s( strBuffer , "SGPS:PATH 0" );
        GPSTester->mp5k_list.at( handle )->Send( strBuffer );
        if ( !GPSTester->mp5k_list.at( handle )->IsWaitTillReadToken( strBuffer , "0" , 200) )
		{
			return MP5K_GPS_PATH_ERROR;
		}
		
		sprintf_s( strBuffer , "SGPS:POWer %f", Setting.OutputPower );
        GPSTester->mp5k_list.at( handle )->Send( strBuffer );
        if ( !GPSTester->mp5k_list.at( handle )->IsWaitTillReadToken( strBuffer , "0" , 200) )
		{
			return MP5K_GPS_POWER_ERROR;
		}
		
		sprintf_s( strBuffer , "SGPS:DOPpler %f",Setting.DopplerHz );
        GPSTester->mp5k_list.at( handle )->Send( strBuffer );
        if ( !GPSTester->mp5k_list.at( handle )->IsWaitTillReadToken( strBuffer , "0" , 200) )
		{
			return MP5K_GPS_DOPPLER_ERROR;
		}
		
		sprintf_s( strBuffer , "SGPS:SVID %i" ,Setting.SVID);
        GPSTester->mp5k_list.at( handle )->Send( strBuffer );
        if ( !GPSTester->mp5k_list.at( handle )->IsWaitTillReadToken( strBuffer , "0" , 200) )
		{
			return MP5K_GPS_SVID_ERROR;
		}
		
		if( Setting.DataPattern == GPS_NVDATA )
		{
			return MP5K_GPS_PATTERN_ERROR;
		}
		sprintf_s( strBuffer , "SGPS:DATA %i" ,Setting.DataPattern);
        GPSTester->mp5k_list.at( handle )->Send( strBuffer );
        if ( !GPSTester->mp5k_list.at( handle )->IsWaitTillReadToken( strBuffer , "0" , 200) )
		{
			return MP5K_GPS_PATTERN_ERROR;
		}
	}
	else
	{
        // write to log
#if MP5KLOG
        sprintf_s(strBuffer, "mp5k_GPS_SingleCh_Setting:[Handle error]", handle);
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
	}
	return MP5K_OK;
}

UINT mp5k_GPS_SingleCh_Start( const UINT& handle )
{
	if ( GPSTester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle != NULL )
    {
		sprintf_s( strBuffer , "SGPS:MODE 1");
        GPSTester->mp5k_list.at( handle )->Send( strBuffer );
        if ( !GPSTester->mp5k_list.at( handle )->IsWaitTillReadToken( strBuffer , "0" ) )
		{
			return MP5K_GPS_RF_ON_ERROR;
		}
	}
	else
	{
        // write to log
#if MP5KLOG
        sprintf_s(strBuffer, "mp5k_GPS_RF_Start:[Handle error]", handle);
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
	}
	return MP5K_OK;
}

UINT mp5k_GPS_SingleCh_Stop( const UINT& handle )
{
	if ( GPSTester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle != NULL )
    {
		sprintf_s( strBuffer , "SGPS:MODE 0");
        GPSTester->mp5k_list.at( handle )->Send( strBuffer );
        if ( !GPSTester->mp5k_list.at( handle )->IsWaitTillReadToken( strBuffer , "0" ) )
		{
			return MP5K_GPS_RF_OFF_ERROR;
		}
	}
	else
	{
        // write to log
#if MP5KLOG
        sprintf_s(strBuffer, "mp5k_GPS_RF_Start:[Handle error]", handle);
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
	}
	return MP5K_OK;
}

UINT mp5k_GPS_CW_Signal( const UINT& handle )
{
	if ( GPSTester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle != NULL )
    {
		sprintf_s( strBuffer , "SGPS:MODE 2");
        GPSTester->mp5k_list.at( handle )->Send( strBuffer );
        if ( !GPSTester->mp5k_list.at( handle )->IsWaitTillReadToken( strBuffer , "\n" ) )
		{
			return MP5K_GPS_CW_SIGNAL_ERROR;
		}
	}
	else
	{
#if MP5KLOG
        sprintf_s(strBuffer, "mp5k_GPS_CW_Signal:[Handle error]", handle);
        Mp5kLog.WriteLine( strBuffer );
#endif
        return MP5K_HANDLE_ERROR;
	}
	return MP5K_OK;
}
	
#endif

UINT mp5k_Cableloss_init( const UINT& handle )
{
	if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_Cableloss_init:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
		return MP5K_HANDLE_ERROR;
	}

    Tester->mp5k_list.at( handle )->Send( "CONFigure:USER:REGister?" );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_HANDLE_ERROR;
    }

    Tester->mp5k_list.at( handle )->Send( "CONFigure:TRIGger:TYPE IMMediate" );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_HANDLE_ERROR;
    }
	
    Tester->mp5k_list.at( handle )->Send( "OUTPut:POWer:LEVel -10" );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_HANDLE_ERROR;
    }
	
    Tester->mp5k_list.at( handle )->Send( "INPut:CAPTure:TIME  0.001" );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_HANDLE_ERROR;
    }
	
    Tester->mp5k_list.at( handle )->Send( "INPut:TXQuality:SKIP:COUNt 5" );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_HANDLE_ERROR;
    }
	
    Tester->mp5k_list.at( handle )->Send( "GPRF" );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_HANDLE_ERROR;
    }
	
    Tester->mp5k_list.at( handle )->Send( "CONFigure:RFCorrection ON" );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_HANDLE_ERROR;
    }

    Tester->mp5k_list.at( handle )->Send( "INPut:LEVel:REFerence -10" );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_HANDLE_ERROR;
    }
	
	return MP5K_OK;

}

UINT mp5k_Cableloss_Meas( const UINT& handle , MP5K_Cableloss_Data& data)
{
	if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_Cableloss_setting:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
		return MP5K_HANDLE_ERROR;
	}
	
	UINT OtherPort[2] = {0,0};
	UINT cnt = 0;
	
	for(int port = 2 ;port < 5; port++)
	{
		if(port != data.Port)
		{
			OtherPort[cnt] = port;
			cnt++;
		}
	}

	sprintf_s( strBuffer,"ROUTe:PORT:RESource RF1,VSA;:ROUTe:PORT:RESource RF%d,VSG;:ROUTe:PORT:RESource RF%d,OFF;:ROUTe:PORT:RESource RF%d,OFF;",
				data.Port, OtherPort[0], OtherPort[1]);

    Tester->mp5k_list.at( handle )->Send( strBuffer );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_HANDLE_ERROR;
    }
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_HANDLE_ERROR;
    }
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_HANDLE_ERROR;
    }
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_HANDLE_ERROR;
    }
	sprintf_s( strBuffer, "INPut:FREQuency %dMHz", data.Frequency );
    Tester->mp5k_list.at( handle )->Send( strBuffer );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_HANDLE_ERROR;
    }
	
    Tester->mp5k_list.at( handle )->Send( "CALCulate:TXQuality:ALL:SYNC" );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_HANDLE_ERROR;
    }

	sprintf_s( strBuffer, "FETCh:RFCorrection:RESource%d:LOSS?", data.Port );
    Tester->mp5k_list.at( handle )->Send( strBuffer );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ) || !SubString( ackBuffer, 2) )
	{
        return MP5K_HANDLE_ERROR;
    }
	data.Cableloss = abs(atof( ackBuffer.c_str() ));
	
	return MP5K_OK;
}

UINT mp5k_Cableloss_close( const UINT& handle)
{
	if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_Cableloss_close:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
		return MP5K_HANDLE_ERROR;
	}
    Tester->mp5k_list.at( handle )->Send( "INPut:TXQuality:SKIP:COUNt 2" );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_HANDLE_ERROR;
    }
	
    Tester->mp5k_list.at( handle )->Send( "CONFigure:RFCorrection OFF" );
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( ackBuffer , "\n" ))
	{
        return MP5K_HANDLE_ERROR;
    }
	return MP5K_OK;
}

UINT mp5k_Send_Cmd( const UINT& handle, std::string& cmd)
{
	if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_Send_Cmd:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
		return MP5K_HANDLE_ERROR;
	}
    Tester->mp5k_list.at( handle )->Send( cmd );
	
	return MP5K_OK;
}

UINT mp5k_Read_Cmd( const UINT& handle, std::string& data, const std::string& token)
{
	if ( Tester == nullptr)
    {
        return MP5K_TESTER_NULL;
    }
    if ( handle == NULL )
    {
#if MP5KLOG
        sprintf_s( strBuffer , "mp5k_Read_Cmd:[Handle error]" , handle );
        Mp5kLog.WriteLine( strBuffer );
#endif
		return MP5K_HANDLE_ERROR;
	}
    if( !Tester->mp5k_list.at( handle )->IsWaitTillReadToken( data , token ))
	{
        return MP5K_HANDLE_ERROR;
    }

	return MP5K_OK;
}
