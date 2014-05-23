#include "io_service.h"
#include "io_service_ref.h"


IOService::IOService(void)
    : work_(new boost::asio::io_service::work(io_service_))
    , strand_(io_service_)
    , thread_(boost::bind(&boost::asio::io_service::run, &io_service_)) {
    // nothing
}

IOService::~IOService(void) {
    delete work_;
    io_service_.stop();
    io_service_.reset();
    thread_.join();
}

boost::asio::io_service& IOService::IO(void) {
    return io_service_;
}



IOServiceRef::IOServiceRef(void)
    : impl_(new IOService()) {
    impl_->AddRef();
}

IOServiceRef::IOServiceRef(const IOServiceRef& other)
    : impl_(other.impl_) {
    impl_->AddRef();
}

IOServiceRef::~IOServiceRef(void) {
    impl_->Release();
}

IOServiceRef& IOServiceRef::operator= (const IOServiceRef& other) {
    (other.impl_)->AddRef();
    impl_->Release();
    impl_ = other.impl_;
    return *this;
}

bool IOServiceRef::operator== (const IOServiceRef& other) const {
    return (impl_ == other.impl_);
}

bool IOServiceRef::operator!= (const IOServiceRef& other) const {
    return (impl_ != other.impl_);
}

IOService* IOServiceRef::operator-> (void) const {
    return impl_;
}