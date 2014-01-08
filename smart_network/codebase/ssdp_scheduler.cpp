#include "ssdp_scheduler.h"


namespace Ssdp {

Scheduler::Scheduler(IOService& io_service)
    : interval_(boost::posix_time::seconds(kSendIntervalSec))
    , timer_(io_service, interval_) {
    // nothing
}

Scheduler::~Scheduler(void) {
    // nothing
}

ScheduleTrigger Scheduler::BindTrigger(ScheduleTrigger f) {
    ScheduleTrigger old = trigger_;
    trigger_ = f;
    return old;
}

void Scheduler::UnbindTrigger(void) {
    trigger_ = 0;
}

void Scheduler::Start(void) {
    timer_.expires_from_now(interval_);
    timer_.async_wait(boost::bind(&Scheduler::handle_fire_trigger, this));
}

void Scheduler::Stop(void) {
    timer_.cancel();
}

void Scheduler::handle_fire_trigger(void) {
    const double time = (double)timer_.expires_at().time_of_day().total_milliseconds() / 1000.0;
    printf("[INFO][Scheduler] search service(s). [%8.3f]\n", time);

    if (!trigger_.empty()) {
        trigger_();
    }

    timer_.expires_at(timer_.expires_at() + interval_);
    timer_.async_wait(boost::bind(&Scheduler::handle_fire_trigger, this));
}

}  // namespace Ssdp