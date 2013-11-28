#ifndef SSDP_H_
#define SSDP_H_
// jh81.kim: SSDP(Simple Service Discovery Protocol) naming form UPnP.
#include "boost_asio_fwd.h"
#include <boost/thread.hpp>
#include <set>


class SsdpSender {
public:
    SsdpSender(IOService& io_service);
    ~SsdpSender(void);

    void RegistTarget(const char* target);
    void UnregistTarget(const char* target);

private:
    void handle_send(void);

private: 
    boost::mutex mutex_;
    std::set<std::string> targets_;

    UDP::socket socket_;
    boost::posix_time::time_duration interval_;
    mutable boost::asio::deadline_timer timer_;
};



class SsdpListener
{
public:
    SsdpListener(IOService& io_service);
    virtual ~SsdpListener(void);

protected:
    // jh81.kim(28No2013): current step, ssdp is just service id.
    virtual void OnSsdp(const std::string& ssdp, const std::string& address) = 0;

private:
    void handle_receive_from(const boost::system::error_code& error, size_t bytes_recvd);

private:
    UDP::socket socket_;
    UDP::endpoint sender_endpoint_;
    enum { max_length = 1024 };
    char data_[max_length];
};


#endif  // SSDP_H_