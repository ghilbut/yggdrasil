#include "http_websocket.h"

#include "http_object.h"
#include <mongoose/mongoose.h>
#include <boost/assert.hpp>
#include <set>


HttpWebsocket::HttpWebsocket(HttpMap& https) 
    : interval_(boost::posix_time::seconds(30))
    , timer_(io_service_, interval_)
    , thread_(boost::bind(&HttpWebsocket::thread_ping, this))
    , https_(https) {

}

HttpWebsocket::~HttpWebsocket(void) {
    io_service_.stop();
    timer_.cancel();
}

void HttpWebsocket::thread_ping(void) {
    timer_.expires_from_now(interval_);
    timer_.async_wait(boost::bind(&HttpWebsocket::Ping, this));
    io_service_.run();
}

void HttpWebsocket::Ping(void) {

    if (!https_.empty()) {
        HttpMap::iterator itr = https_.begin();
        HttpMap::iterator end = https_.end();
        for (; itr != end; ++itr) {
            (itr->second)->PingWebSockets();
        }

        printf("[WP] Ping [%8.3f]\n", (double)timer_.expires_at().time_of_day().total_milliseconds() / 1000.0);
    }

    timer_.expires_at(timer_.expires_at() + interval_);
    timer_.async_wait(boost::bind(&HttpWebsocket::Ping, this));
}