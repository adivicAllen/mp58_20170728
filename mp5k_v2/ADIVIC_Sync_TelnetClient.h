#ifndef ADIVIC_SYNC_TELNET_CLIENT_H
#define ADIVIC_SYNC_TELNET_CLIENT_H

#ifdef VC
    #ifdef _WINDLL
        #define ADIVIC_TCLIENT_API __declspec(dllexport)
    #else
        #define ADIVIC_TCLIENT_API __declspec(dllimport)
    #endif
#else
    #ifdef _WINDLL
    //    #define ADIVIC_TCLIENT_API __attribute__((dllexport))
       #define ADIVIC_TCLIENT_API __attribute__ ((visibility ("default")))
    #else
      //  #define ADIVIC_TCLIENT_API __attribute__((dllimport)))
    #endif
#endif
#include "TOP_LEVEL.h"
#include "Sync_TelnetClient.h"


#define EOL_MARK "\r\n"



struct Sync_TelnetImpl
{
	Sync_TelnetImpl() {};
	Sync_TelnetImpl(const Sync_TelnetImpl &obj)
	{
		ClientPtr = obj.ClientPtr;
		io_ServicePtr = obj.io_ServicePtr;
	}
	~Sync_TelnetImpl()
	{
		ClientPtr.reset();
		io_ServicePtr.reset();
	};
	boost::shared_ptr<Sync_TelnetClient> ClientPtr;
	boost::shared_ptr<boost::asio::io_service> io_ServicePtr;
};


#if MP5800API
class   ADIVIC_TCLIENT_API ADIVIC_Sync_TelnetClient
#else
class    ADIVIC_Sync_TelnetClient
#endif

//__attribute__ ((visibility ("default")))
// class  __attribute__ ((dllexport)) ADIVIC_Sync_TelnetClient
//class  __attribute__ ((visibility ("default"))) ADIVIC_Sync_TelnetClient
{
public:
	ADIVIC_Sync_TelnetClient(const std::string&);
	ADIVIC_Sync_TelnetClient();     // allen 20170126
	~ADIVIC_Sync_TelnetClient();
	bool Open(const std::string& host, const unsigned int port = 5499, const unsigned int timeout_sec = 2);
	void Close();

	void Send(const std::string&);
	bool IsWaitTillReadToken(std::string& ack, const std::string& token, unsigned int timeout_ms = 2000);
	bool IsWaitTillReadToken(char* ack, const std::string& token, unsigned int timeout_ms = 2000);
	bool IsConnected();
	bool WaitToRead( std::string & msg);    // allen 20170126

	std::string ip;

	void CleanQueue();
private:
	Sync_TelnetImpl* m_TelnetImpl;
	bool m_isConnected;
};


#endif
