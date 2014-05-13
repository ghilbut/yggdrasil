#ifndef HTTP_WEBSOCKET_H_
#define HTTP_WEBSOCKET_H_

#include "environ.h"
#include <boost/asio.hpp>


class Environ;
struct mg_connection;

namespace Http {

class Message;

class WebSocket {
public:
    WebSocket(Environ* env, struct mg_connection* conn);
    ~WebSocket(void);

    static void WeakCallback(const v8::WeakCallbackData<v8::Object, WebSocket>& data);
    void MakeWeak(v8::Isolate* isolate, v8::Local<v8::Object> self);
    void ClearWeak(void);

    void DoSend(const Message& msg) const;


private:
    void handle_send(const Message& msg) const;


private:
    v8::Isolate* isolate_;
    v8::Persistent<v8::Object> self_;

    mutable boost::asio::strand strand_;
    struct mg_connection* conn_;
};

}  // namespace Http

#endif  // HTTP_WEBSOCKET_H_