#include "http_websocket_ping_scheduler.h"

#include "http_object.h"
#include <mongoose/mongoose.h>
#include <set>


namespace Http {

WebsocketPingScheduler::WebsocketPingScheduler(IOService& io_service) 
    : interval_(boost::posix_time::seconds(kPingIntervalSec))
    , timer_(io_service, interval_) {
    // nothing
}

WebsocketPingScheduler::~WebsocketPingScheduler(void) {
    Stop();
}

void WebsocketPingScheduler::Register(const Object* object) {
    boost::mutex::scoped_lock lock(mutex_);
    objects_.insert(object);
}

void WebsocketPingScheduler::Unregister(const Object* object) {
    boost::mutex::scoped_lock lock(mutex_);
    objects_.erase(object);
}

void WebsocketPingScheduler::Start(void) {
    timer_.expires_from_now(interval_);
    timer_.async_wait(boost::bind(&WebsocketPingScheduler::handle_ping, this));
}

void WebsocketPingScheduler::Stop(void) {
    timer_.cancel();
}

void WebsocketPingScheduler::handle_ping(void) {

    printf("[WP] Ping [%8.3f]\n", (double)timer_.expires_at().time_of_day().total_milliseconds() / 1000.0);

    if (!objects_.empty()) {
        boost::mutex::scoped_lock(mutex_);
        std::set<const Object*>::iterator itr = objects_.begin();
        std::set<const Object*>::iterator end = objects_.end();
        for (; itr != end; ++itr) {
            const_cast<Object*>(*itr)->PingWebSockets();
        }
    }

    timer_.expires_at(timer_.expires_at() + interval_);
    timer_.async_wait(boost::bind(&WebsocketPingScheduler::handle_ping, this));
}

}  // namespace Http