#ifndef SSDP_H_
#define SSDP_H_
// jh81.kim: SSDP(Simple Service Discovery Protocol) naming form UPnP.
#include "boost_asio_fwd.h"


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