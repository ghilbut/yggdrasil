#ifndef ENVIRON_H_
#define ENVIRON_H_

#include "storage.h"
#include <v8.h>
#include <boost/asio.hpp>


class Environ {
public:
    Environ(boost::asio::io_service& io_service
        , const char* basepath
        , int port);
    ~Environ(void);

    boost::asio::io_service& io_service(void) const;
    Storage& storage(void) const;
    v8::Isolate* isolate(void) const;
    int port(void) const;

private:
    boost::asio::io_service& io_service_;
    v8::Isolate* isolate_;
    mutable Storage storage_;
    const int port_;
};

#endif  // ENVIRON_H_