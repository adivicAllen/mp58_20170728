#include "ADIVIC_Logger.h"


ADIVIC_Logger& ADIVIC_Logger::GetInstance()
{
	static ADIVIC_Logger obj;
	return obj;
}


ADIVIC_Logger::ADIVIC_Logger()
{
	//m_isEnable = false;
}

ADIVIC_Logger::~ADIVIC_Logger()
{
}

void ADIVIC_Logger::CreateLog(const std::string& LogName)
{
	if( LogName.empty() )
	{
		return;
	}


	std::string LogNameTemp = "..\\RF_LOGS\\"+ LogName + "_%Y-%m-%d_%H-%M-%S.log";

	boost::shared_ptr< boost::log::sinks::text_file_backend > backend = 
		boost::make_shared< boost::log::sinks::text_file_backend >(
		boost::log::keywords::file_name = LogNameTemp,
		boost::log::keywords::rotation_size = 10 * 1024 * 1024,
		boost::log::keywords::auto_flush = true	);
	
	boost::shared_ptr< sink_t > sink = boost::make_shared< sink_t >( backend );
	
	sink->set_formatter(
		      	boost::log::expressions::format("[%1%] \t %2%")
            % boost::log::expressions::attr< boost::posix_time::ptime >("TimeStamp")
			% boost::log::expressions::smessage);
	
	#if MP5800API
	sink->locked_backend()->set_file_collector( boost::log::sinks::file::make_collector(
	 	boost::log::keywords::target = "..\\RF_LOGS\\", //Directory
		boost::log::keywords::max_size = 40*1024*1024,
		boost::log::keywords::min_free_space = 100*1024*1024	));
#else
	sink->locked_backend()->set_file_collector( boost::log::sinks::file::make_collector(
		boost::log::keywords::target = ".\\MFG_LOG\\", //Directory
		boost::log::keywords::max_size = 40*1024*1024,
		boost::log::keywords::min_free_space = 100*1024*1024	));

#endif
	
	sink->locked_backend()->scan_for_files();
	

	boost::shared_ptr< boost::log::core > core = boost::log::core::get();
	core->add_sink( sink );
	core->add_global_attribute( "TimeStamp" , boost::log::attributes::local_clock() );
}
