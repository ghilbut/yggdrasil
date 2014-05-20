#ifndef HTTP_WEBSOCKET_H_
#define HTTP_WEBSOCKET_H_

#include "basebox/device_host_ref.h"
#include <v8.h>


struct mg_connection;

namespace Http {

class Message;

class WebSocket {
public:
    WebSocket(void);
    WebSocket(const DeviceRef& device, struct mg_connection* conn);
    WebSocket(const WebSocket& other);
    ~WebSocket(void);

    WebSocket& operator= (WebSocket& other);

    // methods
    void DoSend(const Message& msg) const;
    void FireMessage(const Message& msg) const;
    void FireClosed(void) const;

    // properties
    v8::Local<v8::Object> self(v8::Isolate* isolate) const;
    v8::Local<v8::Object> message_trigger(v8::Isolate* isolate) const;
    void set_message_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);
    v8::Local<v8::Object> closed_trigger(v8::Isolate* isolate) const;
    void set_closed_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);


public:
    class Impl;
    Impl* pimpl_;
};

}  // namespace Http

#endif  // HTTP_WEBSOCKET_H_