#ifndef ADIVIC_LOGGER_H
#define ADIVIC_LOGGER_H

#include "define.h"
#include "MFG_INCLUDE.h"
#include <stdarg.h>
#include <cstdarg>
#define MAX_BUFF 4096

typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_file_backend > sink_t;

class ADIVIC_Logger : public boost::noncopyable
{
public:

    static ADIVIC_Logger& GetInstance();

	~ADIVIC_Logger();
	//static void SetEnable(bool b){ m_isEnable = b; };
	
	void CreateLog(const std::string&);	
    template<typename T>
	inline void WriteLog(const T& log)
	{
#ifdef MP5KLOG
		BOOST_LOG_TRIVIAL(trace) << log;
#endif
	}


	inline void WriteLog(const char* formate, ...)
	{
#ifdef MP5KLOG
		va_list arguments = NULL;
		va_start(arguments, formate);
		char buff[MAX_BUFF] = { 0 };
    //	vsprintf_s(buff, formate, arguments);  //allen 20170720
        vsprintf(buff, formate, arguments);
		va_end(arguments);
		BOOST_LOG_TRIVIAL(trace) << buff;
#endif
	}

private:
	
	ADIVIC_Logger(const ADIVIC_Logger&);
	ADIVIC_Logger();

	

private:
	static bool m_isEnable;
	
};
#endif
