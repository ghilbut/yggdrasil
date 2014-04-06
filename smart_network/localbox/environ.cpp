#include "environ.h"


Environ::Environ(boost::asio::io_service& io_service
                 , const char* basepath
                 , int port)
    : io_service_(io_service)
    , isolate_(v8::Isolate::New())
    , storage_(basepath)
    , port_(port) {
    // nothing
}

Environ::~Environ(void) {
    isolate_->Dispose();
}

boost::asio::io_service& Environ::io_service(void) const {
    return io_service_;
}

Storage& Environ::storage(void) const {
    return storage_;
}

v8::Isolate* Environ::isolate(void) const {
    return isolate_;
}

int Environ::port(void) const {
    return port_;
}