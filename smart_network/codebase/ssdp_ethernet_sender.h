#ifndef SSDP_ETHERNET_SENDER_H_
#define SSDP_ETHERNET_SENDER_H_

#include "ssdp_sender.h"
#include "boost_lib_fwd.h"
#include <set>


namespace Ssdp {

class EthernetSender : public Sender {
public:
    explicit EthernetSender(IOService& io_service);
    ~EthernetSender(void);

    virtual void RegistTarget(const std::string& target);
    virtual void UnregistTarget(const std::string& target);
    virtual void Send(void) const;


private:
    mutable UDP::socket socket_;
    mutable boost::mutex mutex_;
    std::set<std::string> targets_;
};

}  // namespace ssdp

#endif  // SSDP_ETHERNET_SENDER_H_