#ifndef SERVICE_BROKER_IMPL_H_
#define SERVICE_BROKER_IMPL_H_

#include <v8.h>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <map>


class Storage;

class ServiceBroker::Impl {
public:
    Impl(v8::Isolate* isolate, const Storage& storage);
    ~Impl(void);

    void RunShell(void);

private:
    const Storage& storage_;

    //boost::asio::io_service io_service_;
    //boost::asio::io_service::work* work_;
    //boost::thread thread_;

    v8::Isolate* isolate_;
    v8::Isolate::Scope isolate_scope_;
    v8::HandleScope handle_scope_;
    v8::Handle<v8::Context> context_;
};

#endif  // SERVICE_BROKER_IMPL_H_