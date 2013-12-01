#include "service.h"

#include "service_info.h"
#include "http/http_request.h"
#include "codebase/device/device_info.h"
#include <json/json.h>


Service::Service(const DeviceInfo*& device
                 , ServiceInfo*& service
                 , uint32_t port
                 , Channel::Ptr channel)
    : Http::Object(device->uipath(), port)
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

bool Service::DoExecute(mg_connection* conn
                        , const char* method
                        , const char* uri) {
    return false;
}

bool Service::DoRequest(mg_connection* conn
                        , const char* method
                        , const char* uri) {

    boost::uuids::random_generator gen;
    const std::string id = to_string(gen());

    HttpRequest request(*channel_, conn);
    requests_[id] = &request;
    if (!request.DoRequest(id.c_str(), method, uri)) {
        requests_.erase(id);
    }

    // TODO(ghilbut): it seems, return value is not necessary.
    return false;
}

bool Service::DoNotify(const std::string& text) {

    if (channel_.get() != 0) {
        chat_message msg;
        msg.type(chat_message::kNotify);
        const size_t size = text.length();
        msg.body_length(size);
        strcpy(msg.body(), text.c_str());
        msg.encode_header();
        channel_->Deliver(msg);
    }

    // TODO(ghilbut): it seems, return value is not necessary.
    return false;
}

void Service::OnResponse(const std::string& json) {
    printf("[Service][Response] %s\n", json.c_str());

    Json::Reader reader;
    Json::Value root(Json::objectValue);
    if (!reader.parse(json, root, false)) {
        printf("[ERROR][Service::OnResponse] protocol is not json format string.\n");
        return;
    }

    if (!root.isObject()) {
        printf("[ERROR][Service::OnResponse] json root is not object.\n");
        return;
    }

    if (!root.isMember("id")) {
        printf("[ERROR][Service::OnResponse] request/response ID is not exists.\n");
        return;
    }

    Json::Value& value = root["id"];
    if (!value.isString()) {
        printf("[ERROR][Service::OnResponse] request/response ID is not string value.");
        return;
    }

    const std::string& id = root["id"].asString();
    requests_[id]->DoResponse(json);
}

void Service::OnEvent(const std::string& json) {
    printf("[Service][Event] %s\n", json.c_str());

    //FireEvent(json);
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