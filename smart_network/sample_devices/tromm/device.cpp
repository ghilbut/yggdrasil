#include "device.h"
#include "device_impl.h"


Device::Device(boost::asio::io_service& io_service, const std::string& description)
    : impl_(new Impl(io_service, description)) {
    impl_->BindEventConnected(boost::bind(&Device::OnConnected, this, _1));
    impl_->BindEventRequest(boost::bind(&Device::OnRequest, this, _1, _2));
    impl_->BindEventNotify(boost::bind(&Device::OnNotify, this, _1));
    impl_->BindEventError(boost::bind(&Device::OnError, this));
    impl_->BindEventDisconnected(boost::bind(&Device::OnDisconnected, this));
}

Device::~Device(void) {
    impl_->BindEventDisconnected(0);
    impl_->BindEventError(0);
    impl_->BindEventNotify(0);
    impl_->BindEventRequest(0);
    impl_->BindEventConnected(0);
    delete impl_;
}

void Device::FireEvent(const std::string& json) {
    impl_->FireEvent(json);
}

void Device::OnClose(const std::string& address) {
    impl_->OnClose(address);
}