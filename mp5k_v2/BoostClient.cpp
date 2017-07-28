#ifdef VC
  #include "stdafx.h"
#endif

#include "BoostClient.h"

BoostClient::BoostClient( boost::asio::io_service& io_service )
	:stopped(false), clientSocket(io_service), io_service_(io_service), isconnected(false)
{
}
//----------------------------------------------------------------------
void BoostClient::Connect( tcp::resolver::iterator endpoint_iter )
{
	StartConnect( endpoint_iter );
}
//----------------------------------------------------------------------
void BoostClient::Disconnect(void)
{
	isconnected = false;
	stopped = true;
	clientSocket.close();
}
//----------------------------------------------------------------------
void BoostClient::SendString( const string& msg )
{
	io_service_.post( boost::bind( &BoostClient::DoWrite , this , msg ) );
}
//----------------------------------------------------------------------
void BoostClient::StartConnect( tcp::resolver::iterator endpoint_iter )
{
	if (endpoint_iter != tcp::resolver::iterator())
	{
      clientSocket.async_connect(endpoint_iter->endpoint(),
          boost::bind(&BoostClient::HandleConnect, this, _1, endpoint_iter));
    }
	else
	{
      Disconnect(); // return false;
    }
}
//----------------------------------------------------------------------
void BoostClient::HandleConnect( const boost::system::error_code& ec, tcp::resolver::iterator endpoint_iter)
{
	if( stopped ) return;

	if( !clientSocket.is_open() ){
		StartConnect( ++endpoint_iter );
	}else if(ec){
		clientSocket.close();
		StartConnect( ++endpoint_iter );
	}else{
		clientSocket.set_option( boost::asio::ip::tcp::no_delay(true));
		isconnected = true;
		StartRead();
	}
}
//----------------------------------------------------------------------
void BoostClient::StartRead(void)
{
	clientSocket.async_read_some( boost::asio::buffer(read_msg , max_read_length),
		boost::bind(&BoostClient::handle_read, this, 
		boost::asio::placeholders::error , 
		boost::asio::placeholders::bytes_transferred) );
}
//----------------------------------------------------------------------
void BoostClient::handle_read(const boost::system::error_code& ec, size_t bytes_transferred)
{
	if( stopped ) return;
	if(!ec)
	{
		MsgQueue.push( std::string( read_msg , bytes_transferred ) );
		StartRead();
	}
	else
	{
		Disconnect();
	}
}
//----------------------------------------------------------------------
void BoostClient::DoWrite( const string& msg )
{
	bool write_in_progress = !write_msgs.empty();
	write_msgs.push_back( msg );
    if (!write_in_progress)  StartWrite();
}
//----------------------------------------------------------------------
void BoostClient::StartWrite(void)
{
	boost::asio::async_write(clientSocket,
          boost::asio::buffer(write_msgs.front().data(), write_msgs.front().length()),
          boost::bind(&BoostClient::handle_write, this,
          boost::asio::placeholders::error,
		  boost::asio::placeholders::bytes_transferred));
}
//----------------------------------------------------------------------
void BoostClient::handle_write(const boost::system::error_code& ec, size_t bytes_transferred)
{
	if(!ec)
	{
		write_msgs.pop_front();
		if( !write_msgs.empty() ) StartWrite();
	}
	else
	{
		Disconnect();
	}
}
//----------------------------------------------------------------------
void BoostClient::CheckDeadline(void)
{
	/*
	if( stopped ) return;
	if (clientDeadline.expires_at() <= deadline_timer::traits_type::now())
    {
      //clientSocket.close();
		//printf(" I am Timeout\n");
	  Disconnect();
      clientDeadline.expires_at(boost::posix_time::pos_infin);
    }
    clientDeadline.async_wait(boost::bind(&BoostClient::CheckDeadline, this));
	*/
}
