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


private:
    class Impl;
    Impl* pimpl_;
};

#endif  // SERVICE_BROKER_H_