#ifndef HTTP_OBJECT_H_
#define HTTP_OBJECT_H_

#include "http_websocket_manager.h"
#include "boost_lib_fwd.h"
#include <set>
#include <string>
#include <stdint.h>

using namespace Http;


struct mg_context;
struct mg_connection;


namespace Http {

class Object : boost::noncopyable {
protected:
    Object(const std::string& document_root, uint32_t port);
    virtual ~Object(void) {}


public:
    // mongoose
    int  OnBeginRequest(mg_connection* conn);
    void OnEndRequest(const struct mg_connection* conn, int reply_status_code);
    int  OnWebsocketConnect(const struct mg_connection* conn);
    void OnWebsocketReady(struct mg_connection* conn);
    int  OnWebsocketData(struct mg_connection* conn, int bits, char* data, size_t data_len);

    bool Start(void);
    void Stop(void);

    void SendWebSocketData(const std::string& data);
    void PingWebSockets(void);

    uint32_t port(void) const;


protected:
    virtual int OnRequest(mg_connection* conn) = 0;
    virtual void OnWebsocketText(const std::string& text) = 0;


private:
    mg_context* ctx_;
    WebsocketManager websockets_;
    const std::string document_root_;
    const uint32_t port_;
};

}  // namespace Http

#endif  // HTTP_OBJECT_H_