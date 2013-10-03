#include "service.h"

#include "service_info.h"
#include "device/device_info.h"
#include <boost/filesystem.hpp>


Service::Service(const DeviceInfo*& device
                 , ServiceInfo*& service
                 , uint32_t port
                 , Channel::Ptr channel)
    : HttpObject(device->uipath(), port)
    , device_(device)
    , service_(service)
    , channel_(channel) {
}

Service::~Service(void) {
    ServiceInfo::Delete(service_);
}

void Service::BindHandleDisconnected(HandleDisconnected handle) {
    fire_disconnected_ = handle;
}

void Service::BindChannel(Channel::Ptr& channel) {
    channel_ = channel;
    channel_->BindHandleResponse(boost::bind(&Service::OnResponse, this, _1));
    channel_->BindHandleEvent(boost::bind(&Service::OnEvent, this, _1));
    channel_->BindHandleDisconnected(boost::bind(&Service::OnDisconnected, this));
}

void Service::UnbindChannel(void) {
    channel_->BindHandleConnected(0);
    channel_->BindHandleResponse(0);
    channel_->BindHandleEvent(0);
    channel_->BindHandleDisconnected(0);
    channel_.reset();
}

bool Service::DoRequest(mg_connection* conn
                        , const char* method
                        , const char* uri) {

        


    return false;
}

bool Service::DoExecute(mg_connection* conn
                        , const char* method
                        , const char* uri) {
    return false;
}

void Service::OnResponse(const std::string& json) {
    printf("[Service][Response] %s\n", json.c_str());
}

void Service::OnEvent(const std::string& json) {
    printf("[Service][Event] %s\n", json.c_str());
}

void Service::OnDisconnected(void) {
    printf("[Service][Disconnected] %s\n", service_->id());

    if (fire_disconnected_) {
        fire_disconnected_(service_->id());
    }
}

const ServiceInfo& Service::info(void) const {
    return *service_;
}