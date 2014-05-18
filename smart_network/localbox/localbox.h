#ifndef LOCALBOX_H_
#define LOCALBOX_H_

#include "basebox/service_broker.h"
#include "frontend/http_server.h"
#include <v8.h>
#include <boost/asio.hpp>
#include <boost/atomic.hpp>
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

    boost::asio::io_service io_service_;
    boost::asio::io_service::work* work_;
    boost::thread thread_;




    boost::thread thread2_;
    boost::atomic_bool alive_;
    boost::atomic_bool stop_;




    Http::Server* server0_;
    Http::Server* server1_;
    Http::Server* server2_;

    ServiceBroker* service0_;
    ServiceBroker* service1_;
    ServiceBroker* service2_;

    typedef std::map<int, ServiceBroker*> ServiceMap;
    ServiceMap services_;



    std::string rootpath_;
};

#endif  // LOCALBOX_H_
