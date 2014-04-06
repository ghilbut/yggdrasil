#ifndef SERVICE_BROKER_IMPL_H_
#define SERVICE_BROKER_IMPL_H_

#include "environ.h"
#include <v8.h>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <map>


class Storage;

class ServiceBroker::Impl {
public:
    Impl(boost::asio::io_service& io_service
         , const char* basepath
         , int port);
    ~Impl(void);

    void RunShell(void);

private:
    Environ env_;

    const Storage& storage_;
    v8::Handle<v8::Context> context_;
};

#endif  // SERVICE_BROKER_IMPL_H_