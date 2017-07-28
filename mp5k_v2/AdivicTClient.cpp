// AdivicTClient.cpp : Defines the exported functions for the DLL application.

// #include "stdafx.h"  // allen
#include "AdivicTClient.h"
#include "BoostClient.h"
#include <boost/timer.hpp>

struct TelnetImpl{
	TelnetImpl() {};
	TelnetImpl( const TelnetImpl &obj ){
		ClientPtr = obj.ClientPtr;
		ThreadPtr = obj.ThreadPtr;
		ServicePtr = obj.ServicePtr;
	}
	~TelnetImpl()
	{
		ClientPtr.reset();
		ThreadPtr.reset();
		ServicePtr.reset();
	};
	boost::shared_ptr<BoostClient> ClientPtr;
	boost::shared_ptr<boost::thread> ThreadPtr;
	boost::shared_ptr<boost::asio::io_service> ServicePtr;
};

#define TLENET_TIME_OUT_MS 2000 //2 seconds
#define TLENET_TIME_OUT_MS_INTERVAL 10 //ms

AdivicTClient::AdivicTClient( const char* logFileName )
	:pTelnetImpl( nullptr ),
	m_isConnected( false )
{
	m_Log = new Telnet_log( logFileName );
};
//---------------------------------------------------------------------------
AdivicTClient::~AdivicTClient(void)
{
	if( pTelnetImpl != nullptr ) 
	{
		Close();
	}
	if( m_Log != nullptr )
	{
		delete m_Log;
		m_Log = nullptr;
	}
}
//---------------------------------------------------------------------------
bool AdivicTClient::Open(const char *host, const char *port, unsigned int timeout)
{
	if( pTelnetImpl != nullptr ) return true;
	pTelnetImpl = new TelnetImpl();
	pTelnetImpl->ServicePtr.reset( new boost::asio::io_service() );
	
	tcp::resolver resolver(*pTelnetImpl->ServicePtr);
	tcp::resolver::query query( host , port );
	tcp::resolver::iterator piterator = resolver.resolve( query );
	pTelnetImpl->ClientPtr.reset( new BoostClient( *pTelnetImpl->ServicePtr ) );
	
	pTelnetImpl->ClientPtr->Connect( piterator );
	pTelnetImpl->ThreadPtr.reset( new boost::thread( boost::bind( &boost::asio::io_service::run , pTelnetImpl->ServicePtr ) ) );
	boost::timer t;
	bool ok = false;
	do{
		ok = pTelnetImpl->ClientPtr->isconnected;
	}while( !ok && t.elapsed() < timeout );
	
	m_isConnected = ok;

	if( !ok )
		Close();
	else
		ip = host;
	return ok;
}
//---------------------------------------------------------------------------
 void AdivicTClient::Close()
 {
	 if( pTelnetImpl != nullptr )
	 {	
		 pTelnetImpl->ClientPtr->Disconnect();
		 pTelnetImpl->ServicePtr->stop();
		 pTelnetImpl->ThreadPtr->join();
		 delete pTelnetImpl;
		 pTelnetImpl = nullptr;
	 }
 }
 //---------------------------------------------------------------------------
 void AdivicTClient::Send(const char * msg)
 {
	  std::string log = "Send = "+ (std::string)msg;
	  m_Log->WriteLine( log.c_str() );
	 pTelnetImpl->ClientPtr->SendString( string( msg , strlen(msg) ) );
 }
  //---------------------------------------------------------------------------
 void  AdivicTClient::Send(const string & msg)
 {
	 std::string log = "Send = "+msg;
	 m_Log->WriteLine( log.c_str() );
	 pTelnetImpl->ClientPtr->SendString( msg );
 }
 //---------------------------------------------------------------------------
 void  AdivicTClient::Send(const char * msg, const int len)
 {
	 m_Log->WriteLine( "Send"  );
	 m_Log->WriteLine( msg );
	 pTelnetImpl->ClientPtr->SendString( string( msg , len ) );
 }
//---------------------------------------------------------------------------
void AdivicTClient::ReadAllBuffer( std::string & msg)
{
	std::string temp;
	do{
		pTelnetImpl->ClientPtr->MsgQueue.pop( temp );
		msg = msg + temp;
        // Sleep(1);  // allen
         usleep(1 * 1000);  // by linux micro second sleep
	}while( !pTelnetImpl->ClientPtr->MsgQueue.isEmpty() );
	m_Log->WriteLine( "ReadAllBuffer"  );
	m_Log->WriteLine( msg.c_str()  );
}
//---------------------------------------------------------------------------
bool AdivicTClient::WaitToRead( std::string & msg)
{
	return pTelnetImpl->ClientPtr->MsgQueue.timer_pop(  msg , TLENET_TIME_OUT_MS );
}
//---------------------------------------------------------------------------
bool AdivicTClient::IsWaitTillReadToken(char* buf, const char* token, unsigned int timeout_ms)
{
	boost::timer timer1;
	double timer_count;
	std::string msg;
	do
	{
		char buff[ 513 ] = {'\0'};
		string temp;
		if( pTelnetImpl->ClientPtr->MsgQueue.timer_pop(  temp , TLENET_TIME_OUT_MS_INTERVAL ) )
		{
#ifdef VC
			strncpy_s(buff, 513 ,temp.c_str(), temp.length());
#else
            strncpy(buff ,temp.c_str(), temp.length());
#endif
			msg.append(buff);

			int pos = temp.find('\0');
			if( pos != temp.length()-1 && pos!= std::string::npos) 
			{
				pTelnetImpl->ClientPtr->MsgQueue.push_front( std::string( temp , pos+1 , temp.length() -pos+1) );
			}
			if(boost::algorithm::ifind_first(msg, token)) 
			{
				strncpy_s(buf, 513 , msg.c_str(), msg.length());
				m_Log->WriteLine( "IsWaitTillReadToken"  );
				m_Log->WriteLine( buf );

				return true;
			}
		}
		timer_count =  timer1.elapsed() * 1000;
	}while( timer_count < timeout_ms && pTelnetImpl->ClientPtr->isconnected );

	m_isConnected = pTelnetImpl->ClientPtr->isconnected;

	return false;
}
//---------------------------------------------------------------------------
bool AdivicTClient::IsWaitTillReadToken( string & msg, const std::string & token)
{
	boost::timer timer1;
	double timer_count;
	msg.clear();
	do
	{
		char buff[ 513 ] = {'\0'};
		string temp;
		if( pTelnetImpl->ClientPtr->MsgQueue.timer_pop(  temp , TLENET_TIME_OUT_MS_INTERVAL ) )
		{
			strncpy_s(buff, temp.c_str(), temp.length());
			msg.append(buff);
			if(boost::algorithm::ifind_first(msg, token)) 
			{
				m_Log->WriteLine( "IsWaitTillReadToken"  );
				m_Log->WriteLine( msg.c_str() );

				return true;
			}
		}
		
		timer_count =  timer1.elapsed() * 1000;
	}while( timer_count < TLENET_TIME_OUT_MS && pTelnetImpl->ClientPtr->isconnected );

	m_isConnected = pTelnetImpl->ClientPtr->isconnected;
	return false;

}
//---------------------------------------------------------------------------
bool AdivicTClient::IsWaitTillReadTokenForSpectrum(  char* buf, unsigned int buffLens ,const char * token)
{
	boost::timer timer1;
	double timer_count;
	std::string msg;
	do
	{
		char buff[ 513 ] = {'\0'};
		string temp;
		if( pTelnetImpl->ClientPtr->MsgQueue.timer_pop(  temp , TLENET_TIME_OUT_MS_INTERVAL ) )
		{
			strncpy_s( buff, 513 ,temp.c_str(), temp.length() );
			msg.append(buff);

			int pos = temp.find('\0');
			if( pos != temp.length()-1 && pos!= std::string::npos) 
			{
				pTelnetImpl->ClientPtr->MsgQueue.push_front( std::string( temp , pos+1 , temp.length() -pos+1) );
			}
			if(boost::algorithm::ifind_first(msg, token) && pTelnetImpl->ClientPtr->MsgQueue.isEmpty() ) 
			{
				strncpy_s(buf, buffLens , msg.c_str(), msg.length());
				m_Log->WriteLine( "IsWaitTillReadToken"  );
				m_Log->WriteLine( buf );
				return true;
			}
		}
		timer_count =  timer1.elapsed() * 1000;
	}while( timer_count < 10000 && pTelnetImpl->ClientPtr->isconnected );

	m_isConnected = pTelnetImpl->ClientPtr->isconnected;

	return false;
}
//---------------------------------------------------------------------------
bool AdivicTClient::IsConnected()
{
	return m_isConnected;
}
