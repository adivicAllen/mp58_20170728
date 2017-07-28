
// #include "stdafx.h"  //allen
#include "Sync_TelnetClient.h"



Sync_TelnetClient::Sync_TelnetClient(boost::asio::io_service& io_service) :
m_Socket_ptr(io_service), m_ioService_ptr(io_service ),m_deadtimer( m_ioService_ptr,boost::posix_time::seconds(5))
{
	
};
Sync_TelnetClient::~Sync_TelnetClient(){};


bool Sync_TelnetClient::Connect(const std::string& host, const unsigned int port, const unsigned int timeout_sec)
{
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(host), port);
	
	boost::system::error_code ec;
	m_Socket_ptr.connect(ep,ec );
	
	if( ec )
	{
		return false;
	}
	bool ok = false;
	boost::timer t;
	do{
		ok = m_Socket_ptr.is_open();
	} while (!ok && t.elapsed() < timeout_sec);
	
	m_Socket_ptr.set_option(boost::asio::ip::tcp::no_delay(true));
	return ok;
}

void Sync_TelnetClient::Disconnect()
{
	m_Socket_ptr.close();
}

void Sync_TelnetClient::SendCommand(const std::string& cmd)
{
	boost::system::error_code ec;
	m_Socket_ptr.write_some(boost::asio::buffer(cmd),ec );

	if (ec)
	{
		return;
	}	
}

void Sync_TelnetClient::ReadAck()
{
	char ack_Buffer[512] = {"0"};

	this->readWithTimeout(boost::asio::buffer(ack_Buffer, 512));
	
	std::string temp(ack_Buffer,512 );


	m_AckDeque.push_back( temp );
}

template <typename MutableBufferSequence>
void Sync_TelnetClient::readWithTimeout(const MutableBufferSequence& buffers)
{
	boost::optional<boost::system::error_code> timer_result;
	boost::optional<boost::system::error_code> read_result;

    boost::asio::deadline_timer timer(m_ioService_ptr); 
	timer.expires_from_now(boost::posix_time::seconds(5)); 
    timer.async_wait(boost::bind(&Sync_TelnetClient::set_result, this, &timer_result, _1)); 

	
	m_Socket_ptr.async_read_some( buffers, boost::bind( &Sync_TelnetClient::set_result,this,&read_result, _1));

    m_ioService_ptr.reset();
    while (m_ioService_ptr.run_one())
    { 
        if (read_result)
		{
            timer.cancel();
		}
        else if (timer_result)
		{
            m_Socket_ptr.cancel();
		}
    }
	/*  Allen 
	if (*read_result)
        throw boost::system::system_error(*read_result);
	*/
}

void Sync_TelnetClient::set_result(boost::optional<boost::system::error_code>* a, boost::system::error_code b)
{ 
	a->reset(b);
} 
