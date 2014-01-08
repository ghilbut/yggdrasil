#ifndef SSDP_SCHEDULER_H_
#define SSDP_SCHEDULER_H_
// NOTE(ghilbut): SSDP - Simple Service Discovery Protocol
#include "ssdp_ethernet_sender.h"
#include "boost_lib_fwd.h"
#include <set>


namespace Ssdp {

typedef boost::function<void ()> ScheduleTrigger;

class Scheduler {
public:
    enum { kSendIntervalSec = 5 };

public:
    Scheduler(IOService& io_service);
    ~Scheduler(void);

    ScheduleTrigger BindTrigger(ScheduleTrigger f);
    void UnbindTrigger(void);

    void Start(void);
    void Stop(void);


private:
    void handle_fire_trigger(void);


private:
    boost::posix_time::time_duration interval_;
    mutable boost::asio::deadline_timer timer_;
    ScheduleTrigger trigger_;
};

}  // namespace Ssdp 

#endif  // SSDP_SCHEDULER_H_