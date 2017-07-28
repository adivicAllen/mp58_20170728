#ifndef MFG_TOOL_INCLUDE_H
#define MFG_TOOL_INCLUDE_H

#include "types.h"
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

//#include <atlconv.h>

#if !defined(Q_MOC_RUN) 
// /home/allen/boost_1_61_0/boost/timer/timer.hpp

#include <boost/timer.hpp>


#include <boost/noncopyable.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/utility/empty_deleter.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/program_options/detail/utf8_codecvt_facet.hpp>

#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/algorithm/string.hpp>
#endif

#endif
