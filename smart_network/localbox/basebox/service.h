#ifndef SERVICE_H_
#define SERVICE_H_

#include "basebox/device_ref.h"
#include "backend/channel_ref.h"
#include "backend/channel_delegate.h"
#include "frontend/http_request_manager.h"
#include "frontend/http_websocket_manager.h"
#include "base/ref_object.h"
#include <v8.h>


enum mg_event;
struct mg_connection;
class ServiceDesc;

class Service : public RefObject
                , public ChannelDelegate {
public:
    Service(const DeviceRef& device_ref, ServiceDesc* desc);
    ~Service(void);

    void RunShell(void);

    typedef boost::function<void (const char*)> SsdpTrigger;
    void SendSsdp(SsdpTrigger trigger) const;

    int HttpRequest(struct mg_connection* conn, enum mg_event ev);
    void HttpNotify(const Http::Message& msg);

    v8::Local<v8::Object> http_request_trigger(v8::Isolate* isolate) const;
    void set_http_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger);
    v8::Local<v8::Object> websocket_open_trigger(v8::Isolate* isolate) const;
    void set_websocket_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);

    void BindChannel(const ChannelRef& channel);
    void UnbindChannel(void);

    // channel delegate
    virtual void OnChannelOpen(const ChannelRef& channel, const std::string& text);
    virtual void OnChannelRecv(const ChannelRef& channel, const std::string& text);
    virtual void OnChannelClosed(const ChannelRef& channel);

    void ChannelSend(const char* json) const;

    v8::Local<v8::Object> channel_open_trigger(v8::Isolate* isolate) const;
    void set_channel_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);
    v8::Local<v8::Object> channel_recv_trigger(v8::Isolate* isolate) const;
    void set_channel_recv_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);
    v8::Local<v8::Object> channel_closed_trigger(v8::Isolate* isolate) const;
    void set_channel_closed_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);

    const char* id(void) const;

private:
    void event_channel_open(void) const;
    void event_channel_recv(void) const;
    void event_channel_closed(void) const;

private:
    DeviceRef device_;
    ServiceDesc* desc_;
    ChannelRef channel_;

    Http::RequestManager req_manager_;
    Http::WebSocketManager ws_manager_;
    
    v8::Persistent<v8::Object> self_;
    v8::Persistent<v8::Object> http_;
    v8::Persistent<v8::Object> on_channel_open_;
    v8::Persistent<v8::Object> on_channel_recv_;
    v8::Persistent<v8::Object> on_channel_closed_;
};

#endif  // SERVICE_H_