#ifndef SSDP_ETHERNET_SENDER_H_
#define SSDP_ETHERNET_SENDER_H_

#include "boost_lib_fwd.h"
#include <set>


namespace Ssdp {

class EthernetSender {
public:
    explicit EthernetSender(boost::asio::io_service& io_service);
    ~EthernetSender(void);

    void RegistTarget(const std::string& target);
    void UnregistTarget(const std::string& target);
    void Send(void) const;


private:
    mutable UDP::socket socket_;
    mutable boost::mutex mutex_;
    std::set<std::string> targets_;
};

}  // namespace ssdp

#endif  // SSDP_ETHERNET_SENDER_H_