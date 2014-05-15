#ifndef HTTP_WEBSOCKET_IMPL_H_
#define HTTP_WEBSOCKET_IMPL_H_

#include "base/ref_implement.h"
#include "environ.h"
#include <boost/asio.hpp>


class Environ;
struct mg_connection;

namespace Http {

class Message;

class WebSocket::Impl : public RefImplement {
private:
    static void WeakCallback(const v8::WeakCallbackData<v8::Object, Impl>& data);

    Impl(Environ* env, struct mg_connection* conn);
    ~Impl(void);

public:
    static Impl* New(Environ* env, struct mg_connection* conn);

    // methods
    void DoSend(const Message& msg) const;
    void FireMessage(const Message& msg) const;
    void FireClosed(void);

    // properties
    v8::Local<v8::Object> self(v8::Isolate* isolate) const;
    v8::Local<v8::Object> message_trigger(v8::Isolate* isolate) const;
    void set_message_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);
    v8::Local<v8::Object> closed_trigger(v8::Isolate* isolate) const;
    void set_closed_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);


private:
    Environ* env_;
    struct mg_connection* conn_;

    v8::Persistent<v8::Object> self_;
    v8::Persistent<v8::Object> on_message_;
    v8::Persistent<v8::Object> on_closed_;
};

}  // namespace Http

#endif  // HTTP_WEBSOCKET_IMPL_H_