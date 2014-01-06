#include "ssdp_scheduler.h"


namespace Ssdp {

Scheduler::Scheduler(IOService& io_service)
    : interval_(boost::posix_time::seconds(kSendIntervalSec))
    , timer_(io_service, interval_) 
    , eth_sender_(io_service) {

    timer_.expires_from_now(interval_);
    timer_.async_wait(boost::bind(&Scheduler::handle_send, this));
}

Scheduler::~Scheduler(void) {
    // nothing
}

void Scheduler::RegistTarget(const std::string& target) {
    eth_sender_.RegistTarget(target);
}

void Scheduler::UnregistTarget(const std::string& target) {
    eth_sender_.UnregistTarget(target);
}

void Scheduler::handle_send(void) {
    const double time = (double)timer_.expires_at().time_of_day().total_milliseconds() / 1000.0;
    printf("[INFO][Scheduler] search service(s). [%8.3f]\n", time);

    eth_sender_.Send();

    timer_.expires_at(timer_.expires_at() + interval_);
    timer_.async_wait(boost::bind(&Scheduler::handle_send, this));
}

}  // namespace Ssdp