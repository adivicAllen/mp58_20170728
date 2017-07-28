#ifndef ADIVIC_TCLIENT_H
#define ADIVIC_TCLIENT_H

#ifdef VC
    #ifdef _WINDLL
        #define ADIVIC_TCLIENT_API __declspec(dllexport)
    #else
        #define ADIVIC_TCLIENT_API __declspec(dllimport)
    #endif
#else
    #ifdef _WINDLL
        #define ADIVIC_TCLIENT_API __attribute__((dllexport))
    #else
        #define ADIVIC_TCLIENT_API __attribute__((dllimport)))
    #endif
#endif


#include <string>
#include <fstream>

#include <ctime>
#include <unistd.h>
#include <sys/timeb.h>
using namespace std;

/*
#include "BoostClient.h"
#include <string>

#define TLENET_TIME_OUT_MS 10000 //10 seconds
#define TLENET_TIME_OUT_MS_INTERVAL 10 //ms

struct TelnetImpl;
*/

struct TelnetImpl;


class Telnet_log {
    private:
        std::ofstream pLog;
		char buff[1024];
    public:
        inline Telnet_log(const std::string filename)
        {
            // create the file
            WriteOpen( filename );
        };
        inline ~Telnet_log(void)
        {
            // close the file
            WriteClose();
        };
        inline bool WriteOpen( const std::string filename )
        {
			pLog.open( filename , std::ofstream::out);
			return pLog.is_open();
        };
        inline bool WriteClose()
        {
            pLog.close();
            return true;
        };
#ifdef VC
        inline bool WriteLine(const char* message)
        {
            SYSTEMTIME st, lt;
            GetSystemTime(&st);
            GetLocalTime(&lt);       
            sprintf (buff , "[%02d:%02d:%02d:%03d]:[%s]\n" ,
                     lt.wHour ,
                     lt.wMinute ,
                     lt.wSecond ,
                     lt.wMilliseconds ,
                     message
                    );
			pLog << buff;
			pLog.flush();
            return true;
        };
#else
        inline bool WriteLine(const char* message)
        {
          //  SYSTEMTIME st, lt;
           // GetSystemTime(&st);
          //  GetLocalTime(&lt);
              std::time_t lt = std::time(NULL);
              std::tm res = *std::localtime(&lt);
              struct timeb milliSec;
              ftime(&milliSec);

            sprintf (buff , "[%02d:%02d:%02d:%03d]:[%s]\n" ,
                     res.tm_hour,
                     res.tm_min ,
                     res.tm_sec ,
                     milliSec.millitm ,
                     message
                    );
            pLog << buff;
            pLog.flush();
            return true;
        };
#endif


};




class  AdivicTClient{
	
public:
	AdivicTClient(const char* logFileName );
	~AdivicTClient(void);
	bool Open(const char * host, const char * port = "5499", unsigned int timeout = 30);
    void Close(void);
    void Send(const char * msg);
    void Send(const std::string & msg);
	void Send(const char * msg, const int len);
    void ReadAllBuffer( std::string & msg);
    bool WaitToRead( std::string & msg);
    bool IsWaitTillReadToken(char * buf, const char * token, unsigned int timeout_ms = 10000);
	bool IsWaitTillReadTokenForSpectrum( char* buf, unsigned int buffLens ,const char * token);
    bool IsWaitTillReadToken(std::string & msg, const std::string & token);
	bool IsConnected();
	//void Test();
	const char* ip; //
private:
	bool m_isConnected;
	TelnetImpl *pTelnetImpl;

	 Telnet_log* m_Log;
};



#endif
