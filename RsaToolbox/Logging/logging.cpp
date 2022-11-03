#include "logging.hpp"
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>


void init_logging(boost::log::trivial::severity_level level, const char* filename) {
  // set log file, format
  boost::log::add_common_attributes();
  boost::log::add_file_log(
    boost::log::keywords::file_name  = filename,
    boost::log::keywords::auto_flush = true,
    boost::log::keywords::filter     = boost::log::trivial::severity >= level,
    boost::log::keywords::format     = "%TimeStamp% - %Severity% - %Message% - %FUNCTION% - %FILE%(%LINE%)"
  );
}
