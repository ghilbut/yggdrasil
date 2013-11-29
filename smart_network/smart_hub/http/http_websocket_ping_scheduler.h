#ifndef HTTP_WEBSOCKET_PING_SCHEDULER_H_
#define HTTP_WEBSOCKET_PING_SCHEDULER_H_

#include "codebase/boost_lib_fwd.h"
#include <set>


class HttpObject;

namespace Http {

class WebsocketPingScheduler {
public:
    enum { kPingIntervalSec = 30 };

public:
    WebsocketPingScheduler(void);
    ~WebsocketPingScheduler(void);

    void Register(const HttpObject* object);
    void Unregister(const HttpObject* object);

private:
    void thread_ping(void);
    void Ping(void);

private:
    boost::asio::io_service io_service_;
    boost::posix_time::time_duration interval_;
    mutable boost::asio::deadline_timer timer_;
    boost::thread thread_;
    boost::mutex mutex_;

    std::set<const HttpObject*> objects_;
};

}  // namespace Http;

#endif  // HTTP_WEBSOCKET_H_