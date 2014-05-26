#ifndef SERVICE_H_
#define SERVICE_H_

#include "basebox/device_ref.h"
#include "backend/channel_ref.h"
#include "frontend/http_request_manager.h"
#include "frontend/http_websocket_manager.h"
#include "base/ref_object.h"
#include <v8.h>


enum mg_event;
struct mg_connection;
class ServiceDesc;

class Service : public RefObject {
public:
    Service(const DeviceRef& device_ref, ServiceDesc* desc);
    ~Service(void);

    void RunShell(void);

    int HttpRequest(struct mg_connection* conn, enum mg_event ev);
    void HttpNotify(const Http::Message& msg);

    v8::Local<v8::Object> request_trigger(v8::Isolate* isolate) const;
    void set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger);
    v8::Local<v8::Object> open_trigger(v8::Isolate* isolate) const;
    void set_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);

    void BindChannel(const ChannelRef& channel);
    void UnbindChannel(void);

    void ChannelSend(const char* json) const;

    v8::Local<v8::Object> channel_open_trigger(v8::Isolate* isolate) const;
    void set_channel_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);
    v8::Local<v8::Object> channel_recv_trigger(v8::Isolate* isolate) const;
    void set_channel_recv_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);
    v8::Local<v8::Object> channel_closed_trigger(v8::Isolate* isolate) const;
    void set_channel_closed_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);

    const char* id(void) const;


private:
    DeviceRef device_;
    ServiceDesc* desc_;

    Http::RequestManager req_manager_;
    Http::WebSocketManager ws_manager_;
    ChannelRef channel_;
    
    v8::Persistent<v8::Object> self_;
    v8::Persistent<v8::Object> http_;

    v8::Persistent<v8::Object> on_channel_open_;
    v8::Persistent<v8::Object> on_channel_recv_;
    v8::Persistent<v8::Object> on_channel_closed_;
};

#endif  // SERVICE_H_