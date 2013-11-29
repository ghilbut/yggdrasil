#ifndef HTTP_WEBSOCKET_H_
#define HTTP_WEBSOCKET_H_

#include "codebase/boost_lib_fwd.h"
#include <set>
#include <string>
#include <stdint.h>


struct mg_context;
struct mg_connection;
class HttpObject;
typedef std::map<const struct mg_context*, HttpObject*> HttpMap;

class HttpWebsocket {
public:
    enum { kPingIntervalSec = 30 };

public:
    explicit HttpWebsocket(HttpMap& https);
    ~HttpWebsocket(void);

    void thread_ping(void);
    void Ping(void);


private:
    boost::asio::io_service io_service_;
    boost::posix_time::time_duration interval_;
    mutable boost::asio::deadline_timer timer_;
    boost::thread thread_;

    HttpMap& https_;
};

#endif  // HTTP_WEBSOCKET_H_