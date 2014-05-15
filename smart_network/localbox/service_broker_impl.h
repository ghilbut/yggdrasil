#ifndef SERVICE_BROKER_IMPL_H_
#define SERVICE_BROKER_IMPL_H_

#include "environ.h"
#include "frontend/http_request_manager.h"
#include "frontend/http_websocket_manager.h"
#include <v8.h>
#include <boost/atomic.hpp>


enum mg_event;
struct mg_connection;
class Storage;

class ServiceBroker::Impl {
public:
    Impl(boost::asio::io_service& io_service
         , const char* basepath
         , int port);
    ~Impl(void);

    void RunShell(void);



    
    int HttpRequest(struct mg_connection* conn, enum mg_event ev);
    void HttpPause(void);
    void HttpResume(void);

    v8::Local<v8::Object> request_trigger(v8::Isolate* isolate) const;
    void set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger);
    v8::Local<v8::Object> open_trigger(v8::Isolate* isolate) const;
    void set_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);


private:
    Environ env_;

    const Storage& storage_;
    v8::Handle<v8::Context> context_;






    

    
    boost::atomic_bool http_paused_;
    Http::RequestManager req_manager_;
    Http::WebSocketManager ws_manager_;
    
    v8::Persistent<v8::Object> http_;
};

#endif  // SERVICE_BROKER_IMPL_H_