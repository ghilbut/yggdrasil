#include "http_websocket_ping_scheduler.h"

#include "http_object.h"
#include <mongoose/mongoose.h>
#include <set>


namespace Http {

WebsocketPingScheduler::WebsocketPingScheduler(void) 
    : interval_(boost::posix_time::seconds(kPingIntervalSec))
    , timer_(io_service_, interval_)
    , thread_(boost::bind(&WebsocketPingScheduler::thread_ping, this)) {
    // nothing
}

WebsocketPingScheduler::~WebsocketPingScheduler(void) {
    io_service_.stop();
    timer_.cancel();
    thread_.join();
}

void WebsocketPingScheduler::Register(const HttpObject* object) {
    boost::mutex::scoped_lock lock(mutex_);
    objects_.insert(object);
}

void WebsocketPingScheduler::Unregister(const HttpObject* object) {
    boost::mutex::scoped_lock lock(mutex_);
    objects_.erase(object);
}

void WebsocketPingScheduler::thread_ping(void) {
    timer_.expires_from_now(interval_);
    timer_.async_wait(boost::bind(&WebsocketPingScheduler::Ping, this));
    io_service_.run();
}

void WebsocketPingScheduler::Ping(void) {
    boost::mutex::scoped_lock lock(mutex_);

    if (!objects_.empty()) {
        std::set<const HttpObject*>::iterator itr = objects_.begin();
        std::set<const HttpObject*>::iterator end = objects_.end();
        for (; itr != end; ++itr) {
            const_cast<HttpObject*>(*itr)->PingWebSockets();
        }

        printf("[WP] Ping [%8.3f]\n", (double)timer_.expires_at().time_of_day().total_milliseconds() / 1000.0);
    }

    timer_.expires_at(timer_.expires_at() + interval_);
    timer_.async_wait(boost::bind(&WebsocketPingScheduler::Ping, this));
}

}  // namespace Http