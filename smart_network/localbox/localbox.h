#ifndef LOCALBOX_H_
#define LOCALBOX_H_

#include "service_broker.h"
#include <v8.h>
#include <boost/asio.hpp>
#include <boost/thread.hpp>


class LocalBox {
public:
    LocalBox(const char* rootpath);
    ~LocalBox(void);

    void RunShell(void);

private:
    v8::Isolate* isolate_;
    v8::HandleScope handle_scope_;
    v8::Handle<v8::Context> context_;

    boost::asio::io_service io_service_;
    boost::asio::io_service::work* work_;
    boost::thread thread_;

    ServiceBroker* service0_;
    ServiceBroker* service1_;
    ServiceBroker* service2_;
};

#endif  // LOCALBOX_H_