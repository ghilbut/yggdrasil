#include "service.h"

#include "service_desc.h"
#include "http/http_request.h"
#include "codebase/device/device_desc.h"
#include <json/json.h>


Service::Service(const DeviceDesc*& device
                 , ServiceDesc*& service
                 , uint32_t port)
    : Http::Object(device->uiroot(), port)
    , device_(device)
    , service_(service) {
}

Service::~Service(void) {
    ServiceDesc::Delete(service_);
}

void Service::BindChannel(Channel* channel) {
    channel_ = channel;
    channel->BindDelegate(this);
}

void Service::UnbindChannel(void) {
    channel->UnbindDelegate(this);
    //channel_.reset();
}

bool Service::FireRequest(mg_connection* conn
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

bool Service::FireNotify(const std::string& text) {

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

const ServiceDesc& Service::info(void) const {
    return *service_;
}