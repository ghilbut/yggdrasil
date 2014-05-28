#ifndef SSDP_ETHERNET_SENDER_H_
#define SSDP_ETHERNET_SENDER_H_

#include "boost_lib_fwd.h"
#include <set>


namespace Ssdp {

class EthernetSender {
public:
    explicit EthernetSender(boost::asio::io_service& io_service);
    ~EthernetSender(void);

    void Send(const char* target) const;


private:
    mutable UDP::socket socket_;
};

}  // namespace ssdp

#endif  // SSDP_ETHERNET_SENDER_H_