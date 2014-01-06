#include "service_broker.h"
#include "service/service_desc.h"
#include "codebase/device/device_desc.h"
#include "codebase/http_request.h"
#include "codebase/chat_message.h"
#include "codebase/channel.h"
#include <json/json.h>


ServiceBroker::ServiceBroker(const DeviceDesc& device_desc, ServiceDesc& service_desc, uint32_t port)
    : Http::UIObject(device_desc.uiroot(), port) 
    , device_desc_(device_desc)
    , service_desc_(service_desc)
    , channel_(0) {
    // nothing
}

void ServiceBroker::OnConnected(const std::string& json, Channel* channel) {
    BOOST_ASSERT(false);
}

void ServiceBroker::OnResponse(const std::string& json) {

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

void ServiceBroker::OnEvent(const std::string& text) {
    FireEvent(text);
}

void ServiceBroker::OnDisconnected(void) {
    delete channel_;
    channel_ = 0;

    if (handle_disconnected_) {
        handle_disconnected_(service_desc_.id());
    }
}

void ServiceBroker::BindChannel(Channel* channel) {
    channel_ = channel;
    channel_->BindDelegate(this);
}

void ServiceBroker::BindDisconnectedHandler(HandleDisconnected handle) {
    BOOST_ASSERT(handle != 0);
    handle_disconnected_ = handle;
}

const ServiceDesc& ServiceBroker::desc(void) const {
    return service_desc_;
}

bool ServiceBroker::FireRequest(mg_connection* conn
                               , const char* method
                               , const char* query) {

    boost::uuids::random_generator gen;
    const std::string id = to_string(gen());

    HttpRequest request(*channel_, conn);
    requests_[id] = &request;
    if (!request.DoRequest(id.c_str(), method, query)) {
        requests_.erase(id);
    }

    // TODO(ghilbut): it seems, return value is not necessary.
    return false;
}

bool ServiceBroker::FireNotify(const std::string& text) {

    if (channel_ != 0) {
        chat_message msg;
        msg.type(chat_message::kNotify);
        const size_t size = text.length();
        msg.body_length(size);
        strcpy(msg.body(), text.c_str());
        msg.encode_header();
        channel_->Deliver(msg);
    }

    return false;
}