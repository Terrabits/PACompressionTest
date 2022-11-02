#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>


// macros
#define LOG(severity_)                       \
  BOOST_LOG_TRIVIAL(severity_)               \
  << boost::log::add_value("FILE", __FILE__) \
  << boost::log::add_value("LINE", __LINE__) \
  << boost::log::add_value("FUNCTION", BOOST_CURRENT_FUNCTION)


// init log
void init_logging(boost::log::trivial::severity_level = boost::log::trivial::info, const char* filename = "main.log");
