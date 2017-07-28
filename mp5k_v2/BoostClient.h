#ifndef BOOST_CLIENT_H
#define BOOST_CLIENT_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/timer.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/atomic.hpp>

#include <queue>
#include <string>

using std::string;
using boost::asio::deadline_timer;
using boost::asio::ip::tcp;

template<typename Type> class aSafeSyncQueue
{
public:
	void push_front( Type const& data )
	{
		queueMutex.lock();
		syncqueue.push_front( data );
		queueMutex.unlock();
		queue_cond.notify_all();
	}

	void push( Type const& data )
	{
		boost::mutex::scoped_lock lock( queueMutex );
		syncqueue.push_back( data );
		queue_cond.notify_all();
	}
	//-----------------------------------------------------------------
	bool isEmpty(void) const
	{
		boost::mutex::scoped_lock lock( queueMutex );
		return syncqueue.empty();
	}
	//-----------------------------------------------------------------
	void clear(void)
	{
		boost::mutex::scoped_lock lock( queueMutex );
		syncqueue.clear();
	}
	//-----------------------------------------------------------------
	void pop( Type &popped_data )
	{
		boost::mutex::scoped_lock lock( queueMutex );
		if( syncqueue.empty() ) return;
		popped_data = syncqueue.front();
		syncqueue.pop_front();
	}
	//-----------------------------------------------------------------
	void wait_pop( Type &popped_data )
	{
		boost::mutex::scoped_lock lock( queueMutex );
		if( syncqueue.empty() ) queue_cond.wait(lock);
		popped_data = syncqueue.front();
		syncqueue.pop_front();
	}
	//-----------------------------------------------------------------
	bool timer_pop( Type &popped_data , unsigned int timeout_millisec )
	{
		bool ok = false;
		boost::mutex::scoped_lock lock( queueMutex );
		while( syncqueue.empty() )
		{
			if( !queue_cond.timed_wait( lock , boost::posix_time::millisec(timeout_millisec) ) ) return false;
			//queue_cond.wait(lock);
		}
		popped_data = syncqueue.front();
		syncqueue.pop_front();
		return true;
	}
	//-----------------------------------------------------------------
private:
	std::deque<Type> syncqueue;
	mutable boost::mutex queueMutex;
	boost::condition_variable queue_cond;
};

//----------------------------------------------------------------------------------------------------------------------
class BoostClient
{
	enum{ max_read_length = 512 };

public:
	BoostClient( boost::asio::io_service& io_service );
	void Connect( tcp::resolver::iterator endpoint_iter );
	void Disconnect(void);
	void SendString( const string& msg );
private:
	void StartConnect( tcp::resolver::iterator endpoint_iter );
	void HandleConnect( const boost::system::error_code& ec, tcp::resolver::iterator endpoint_iter);
	void StartRead(void);
	void handle_read(const boost::system::error_code& ec, size_t bytes_transferred);
	void DoWrite( const string& msg );
	void StartWrite(void);
	void handle_write(const boost::system::error_code& ec, size_t bytes_transferred);
	void CheckDeadline(void);

public:
		aSafeSyncQueue<std::string> MsgQueue;
		boost::atomic<bool> isconnected;
private:
	bool stopped;
	boost::asio::io_service& io_service_;
	tcp::socket clientSocket;
	//deadline_timer clientDeadline;
	
	char read_msg[ max_read_length ];
#ifdef VC
	std::deque<const std::string> write_msgs;
#else
    std::deque< std::string > write_msgs;
#endif
};

#endif
