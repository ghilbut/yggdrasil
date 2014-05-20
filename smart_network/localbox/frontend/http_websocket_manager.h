#ifndef HTTP_WEBSOCKET_MANAGER_H_
#define HTTP_WEBSOCKET_MANAGER_H_

#include "http_websocket.h"
#include "basebox/device_ref.h"
#include <v8.h>
#include <map>


struct mg_connection;

namespace Http {

class Message;
class WebSocket;

class WebSocketManager {
public:
    WebSocketManager(const DeviceRef& device, v8::Persistent<v8::Object>& caller);
    ~WebSocketManager(void);

    void HandleMessage(mg_connection* conn);
    void DoNotify(const Message& msg);

    v8::Local<v8::Object> open_trigger(v8::Isolate* isolate) const;
    void set_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);
    v8::Local<v8::Object> message_trigger(v8::Isolate* isolate) const;
    void set_message_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);
    v8::Local<v8::Object> closed_trigger(v8::Isolate* isolate) const;
    void set_closed_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);

private:
    void event_open(mg_connection* conn);
    void event_message(mg_connection* conn, const Message& msg);
    void event_closed(mg_connection* conn);
    void trigger_notify(const Message& msg);


private:
    DeviceRef device_;

    v8::Persistent<v8::Object>& caller_;
    v8::Persistent<v8::Object> on_open_;
    v8::Persistent<v8::Object> on_message_;
    v8::Persistent<v8::Object> on_closed_;

    typedef std::map<mg_connection*, WebSocket> WebSocketMap;
    WebSocketMap websockets_;
};

}  // namespace Http

#endif  // HTTP_WEBSOCKET_MANAGER_H_