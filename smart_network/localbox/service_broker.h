#ifndef SERVICE_BROKER_H_
#define SERVICE_BROKER_H_

#include <v8.h>
#include <boost/asio.hpp>
#include <string>
#include <stdint.h>


class Storage;

class ServiceBroker {
public:
    ServiceBroker(boost::asio::io_service& io_service
                  , const char* basepath
                  , int port);
    ~ServiceBroker(void);

    void RunShell(void);

    int HttpRequest(struct mg_connection* conn, enum mg_event ev);
    void HttpPause(void);
    void HttpResume(void);

    v8::Local<v8::Object> request_trigger(v8::Isolate* isolate) const;
    void set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger);
    v8::Local<v8::Object> open_trigger(v8::Isolate* isolate) const;
    void set_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger);


private:
    class Impl;
    Impl* pimpl_;
};

#endif  // SERVICE_BROKER_H_