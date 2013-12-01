#ifndef HTTP_WEBSOCKET_PING_SCHEDULER_H_
#define HTTP_WEBSOCKET_PING_SCHEDULER_H_

#include "codebase/boost_lib_fwd.h"
#include <set>


namespace Http {

class Object;

class WebsocketPingScheduler {
public:
    enum { kPingIntervalSec = 30 };

public:
    explicit WebsocketPingScheduler(IOService& io_service);
    ~WebsocketPingScheduler(void);

    void Register(const Http::Object* object);
    void Unregister(const Http::Object* object);

    void Start(void);
    void Stop(void);

private:
    void handle_ping(void);

private:
    boost::posix_time::time_duration interval_;
    mutable boost::asio::deadline_timer timer_;

    boost::mutex mutex_;
    std::set<const Http::Object*> objects_;
};

}  // namespace Http;

#endif  // HTTP_WEBSOCKET_H_