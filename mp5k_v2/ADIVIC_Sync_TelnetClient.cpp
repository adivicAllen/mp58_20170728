// #include "stdafx.h"  // allen20170721
#include "ADIVIC_Sync_TelnetClient.h"
#include "ADIVIC_Logger.h"

ADIVIC_Sync_TelnetClient::ADIVIC_Sync_TelnetClient(const std::string& LogName):
m_TelnetImpl(nullptr),
m_isConnected( false )
{
#if MP5KLOG
	ADIVIC_Logger::GetInstance().CreateLog(LogName);
	ADIVIC_Logger::GetInstance().WriteLog("open 2\n" );
#endif
}
ADIVIC_Sync_TelnetClient::ADIVIC_Sync_TelnetClient():
m_TelnetImpl(nullptr),
m_isConnected( false )
{
	ADIVIC_Logger::GetInstance().CreateLog("MP5000_CMD");
	ADIVIC_Logger::GetInstance().WriteLog("open 1\n" );
}
ADIVIC_Sync_TelnetClient::~ADIVIC_Sync_TelnetClient()
{
}

bool ADIVIC_Sync_TelnetClient::Open(const std::string& host, const unsigned int port, const unsigned int timeout_sec)
{
	if (m_TelnetImpl != nullptr)
	{
		return true;
	}

	m_TelnetImpl = new Sync_TelnetImpl();
	m_TelnetImpl->io_ServicePtr.reset( new boost::asio::io_service());
	m_TelnetImpl->ClientPtr.reset( new Sync_TelnetClient( *(m_TelnetImpl->io_ServicePtr.get()) ));
	
	m_isConnected = m_TelnetImpl->ClientPtr->Connect(host, port, timeout_sec);
	
	ADIVIC_Logger::GetInstance().WriteLog("open \n" );
	if ( !m_isConnected )
	{
		/**/
	}
	ip = host;
	return m_isConnected;
}

void ADIVIC_Sync_TelnetClient::Close()
{
	if (m_TelnetImpl != nullptr)
	{
		ADIVIC_Logger::GetInstance().WriteLog("close \n" );
		m_TelnetImpl->ClientPtr->Disconnect();
		m_TelnetImpl->io_ServicePtr->stop();

		delete m_TelnetImpl;
		m_TelnetImpl = nullptr;
		m_isConnected = false;
	}
}

void ADIVIC_Sync_TelnetClient::Send(const std::string& cmd)
{
	std::string CurrentCmd = cmd + EOL_MARK;
	std::string CMD_Log = "Send: " + CurrentCmd;
	ADIVIC_Logger::GetInstance().WriteLog(CMD_Log.c_str() );
	m_TelnetImpl->ClientPtr->SendCommand( CurrentCmd );
}

bool ADIVIC_Sync_TelnetClient::IsWaitTillReadToken(std::string& ack, const std::string& token, const unsigned int timeout_ms)
{
	boost::timer timeout_timer;
	double timer_count = 0.00;
	std::string ack_temp;
	do
	{
		if( m_TelnetImpl->ClientPtr->m_AckDeque.empty() )
		{
			m_TelnetImpl->ClientPtr->ReadAck();
		}

		ack_temp = m_TelnetImpl->ClientPtr->m_AckDeque.front();
		m_TelnetImpl->ClientPtr->m_AckDeque.pop_front();
		int pos = ack_temp.find('\0');
		if( pos != ack_temp.length()-1 && pos!= std::string::npos) 
		{
			m_TelnetImpl->ClientPtr->m_AckDeque.push_front( std::string( ack_temp , pos+1 , ack_temp.length() -pos+1) );
		}
		ack_temp = ack_temp.substr( 0, pos );

		if (boost::algorithm::ifind_first(ack_temp, token))
		{
			ack.assign( ack_temp );
			std::string CMD_Log = "Receive: " + ack + "\n" ;
			ADIVIC_Logger::GetInstance().WriteLog(CMD_Log.c_str() );
			return true;
		}
		timer_count = timeout_timer.elapsed() * 1000;
	} while (timer_count < timeout_ms);
	ADIVIC_Logger::GetInstance().WriteLog("Receive TimeOut\n" );
	return false;
	
}


bool ADIVIC_Sync_TelnetClient::IsWaitTillReadToken(char* ack, const std::string& token, const unsigned int timeout_ms)
{
	boost::timer timeout_timer;
	double timer_count = 0.00;
	std::string ack_temp;

	do
	{
		if( m_TelnetImpl->ClientPtr->m_AckDeque.empty() )
		{
			m_TelnetImpl->ClientPtr->ReadAck();
		}

		ack_temp = m_TelnetImpl->ClientPtr->m_AckDeque.front();
		m_TelnetImpl->ClientPtr->m_AckDeque.pop_front();
		int pos = ack_temp.find('\0');
		if( pos != ack_temp.length()-1 && pos!= std::string::npos) 
		{
			m_TelnetImpl->ClientPtr->m_AckDeque.push_front( std::string( ack_temp , pos+1 , ack_temp.length() -pos+1) );
		}


		ack_temp.append(ack_temp);
		int count = ack_temp.find("\n");
		size_t n = std::count(ack_temp.begin(), ack_temp.end(), '\n');

		if (boost::algorithm::ifind_first(ack_temp, token))
		{
            // strncpy_s(ack, 513 , ack_temp.c_str(), ack_temp.length());  // allen windows
            strncpy(ack, ack_temp.c_str(), ack_temp.length());
			return true;
		}
		timer_count = timeout_timer.elapsed() * 1000;
	} while (timer_count < timeout_ms);

	return false;
	
}

bool ADIVIC_Sync_TelnetClient::IsConnected()
{
	return m_isConnected;
}

void ADIVIC_Sync_TelnetClient::CleanQueue()
{
	while( !m_TelnetImpl->ClientPtr->m_AckDeque.empty() )		
	{
		m_TelnetImpl->ClientPtr->m_AckDeque.pop_front();
	}
}

 bool ADIVIC_Sync_TelnetClient::WaitToRead( std::string & msg)
 {
	 return IsWaitTillReadToken(msg, "\n", 500);
 }
