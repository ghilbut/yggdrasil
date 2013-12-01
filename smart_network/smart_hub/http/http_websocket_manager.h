#ifndef HTTP_WEBSOCKET_MANAGER_H_
#define HTTP_WEBSOCKET_MANAGER_H_

#include "codebase/boost_lib_fwd.h"
#include <set>


struct mg_connection;

namespace Http {

class WebsocketManager {
public:
    WebsocketManager(void);
    ~WebsocketManager(void);

    void Register(struct mg_connection* conn);
    void Unregister(struct mg_connection* conn);

    void SendText(const std::string& text);
    void Ping(void);


private:
    void SendData(int opcode, const char* data, size_t data_len);

private:
    std::set<struct mg_connection*> websockets_;
    boost::mutex mutex_;
};

}

#endif  // HTTP_WEBSOCKET_MANAGER_H_