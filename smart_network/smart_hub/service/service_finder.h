#ifndef SERVICE_FINDER_H_
#define SERVICE_FINDER_H_

#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>
#include <string>
#include <set>


class ServiceFinder {
    typedef boost::asio::ip::udp UDP;
public:
    ServiceFinder(boost::asio::io_service& io_service, int port);
    ~ServiceFinder(void);

    void RegistTarget(const char* target);
    void UnregistTarget(const char* target);

    void handle_find(void);


private: 
    boost::mutex mutex_;
    std::set<std::string> targets_;
    UDP::socket socket_;

    boost::posix_time::time_duration interval_;
    mutable boost::asio::deadline_timer timer_;
};


#endif  // SERVICE_FINDER_H_