#ifndef SYNC_TELNET_CLIENT_H
#define SYNC_TELNET_CLIENT_H

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/optional.hpp>


class Sync_TelnetClient
{
public:
	Sync_TelnetClient(boost::asio::io_service& );
	~Sync_TelnetClient();

	bool Connect(const std::string& host, const unsigned int port = 5499, const unsigned int timeout_sec = 2);
	void Disconnect();

	void SendCommand(const std::string& );
	void ReadAck();
	void Read_handler(const boost::system::error_code& ec, size_t bytes_transferred);

	void ReadTimeOut();

	template <typename MutableBufferSequence>
	void readWithTimeout(const MutableBufferSequence& buffers);
	

	 void set_result(boost::optional<boost::system::error_code>* , boost::system::error_code); 


	std::deque<std::string>			m_AckDeque;
	char m_ackBuffer[512];

private:
	boost::asio::io_service&		m_ioService_ptr;
	boost::asio::ip::tcp::socket	m_Socket_ptr;
	boost::asio::deadline_timer		m_deadtimer;
};

#endif
