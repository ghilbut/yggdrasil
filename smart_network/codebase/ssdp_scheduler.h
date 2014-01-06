#ifndef SSDP_SCHEDULER_H_
#define SSDP_SCHEDULER_H_
// NOTE(jh81.kim): SSDP - Simple Service Discovery Protocol
#include "ssdp_ethernet_sender.h"
#include "boost_lib_fwd.h"
#include <set>


namespace Ssdp {

class Scheduler {
public:
    enum { kSendIntervalSec = 5 };

public:
    Scheduler(IOService& io_service);
    ~Scheduler(void);

    void RegistTarget(const std::string& target);
    void UnregistTarget(const std::string& target);

private:
    void handle_send(void);


private:
    boost::posix_time::time_duration interval_;
    mutable boost::asio::deadline_timer timer_;

    Ssdp::EthernetSender eth_sender_;
};

}  // namespace Ssdp 

#endif  // SSDP_SCHEDULER_H_