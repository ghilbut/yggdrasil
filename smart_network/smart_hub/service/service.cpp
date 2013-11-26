#include "service.h"

#include "service_info.h"
#include "codebase/device/device_info.h"
#include <json/json.h>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>


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

    Json::Value params(Json::objectValue);
    Json::Value root(Json::objectValue);
    root["id"] = to_string(boost::uuids::random_generator()());
    root["query"] = uri;
    root["params"] = params;

    Json::FastWriter writer;
    const std::string json = writer.write(root);

    chat_message msg;
    const size_t size = json.length();
    msg.body_length(size);
    strncpy(msg.body(), json.c_str(), size);
    msg.encode_header();
    msg.type(chat_message::kRequest);
    channel_->Deliver(msg);

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