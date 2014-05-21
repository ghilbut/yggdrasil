#ifndef SERVICE_BROKER_IMPL_H_
#define SERVICE_BROKER_IMPL_H_

#include "service_broker.h"

#include "basebox/device_ref.h"
#include "frontend/http_request_manager.h"
#include "frontend/http_websocket_manager.h"
#include <v8.h>


enum mg_event;
struct mg_connection;
class Storage;

class ServiceBroker::Impl {
public:
    Impl(const DeviceRef& device_ref);
    ~Impl(void);

    void RunShell(void);



    
    int HttpRequest(struct mg_connection* conn, enum mg_event ev);
    void HttpNotify(const Http::Message& msg);

    v8::Local<v8::Object> request_trigger(v8::Isolate* isolate) const;
    void set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger);
    v8::Local<v8::Object> open_trigger(v8::Isolate* isolate) const;
    void set_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);


private:
    DeviceRef device_;

    const Storage& storage_;

    Http::RequestManager req_manager_;
    Http::WebSocketManager ws_manager_;
    
    v8::Persistent<v8::Object> self_;
    v8::Persistent<v8::Object> http_;
};

#endif  // SERVICE_BROKER_IMPL_H_