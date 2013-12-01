#ifndef BOOST_LIB_FWD_H_
#define BOOST_LIB_FWD_H_


#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

typedef boost::asio::io_service     IOService;
typedef boost::asio::ip::tcp        TCP;
typedef boost::asio::ip::udp        UDP;


#endif  // BOOST_LIB_FWD_H_