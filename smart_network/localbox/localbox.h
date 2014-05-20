#ifndef LOCALBOX_H_
#define LOCALBOX_H_

#include "server_manager.h"
#include "base/io_service_ref.h"
#include "basebox/service_broker.h"
#include "frontend/http_server.h"
#include <v8.h>
#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <map>


enum mg_event;
struct mg_connection;

class LocalBox {
public:
    LocalBox(const char* rootpath);
    ~LocalBox(void);

    int HttpRequest(struct mg_connection* conn, enum mg_event ev);

    void RunShell(void);



    void thread2_main(void);



private:
    v8::Isolate* isolate_;
    v8::HandleScope handle_scope_;
    v8::Handle<v8::Context> context_;

    IOServiceRef io_service_;
    ServerManager servers_;

    ServiceBroker* service0_;
    ServiceBroker* service1_;
    ServiceBroker* service2_;


    std::string rootpath_;    
};

#endif  // LOCALBOX_H_
