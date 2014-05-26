#include "service.h"
#include "service_ref.h"

#include "sample.h"
#include "template_factory.h"
#include "service_desc.h"
#include "basebox/device.h"
#include "backend/channel.h"
#include "frontend/http_object_template.h"
#include "frontend/http_request.h"
#include <mongoose.h>



Service::Service(const DeviceRef& device_ref, ServiceDesc* desc)
    : device_(device_ref)
    , desc_(desc)
    , req_manager_(device_, http_)
    , ws_manager_(device_, self_, http_) {

    v8::Local<v8::Context> context = device_->context();
    v8::Isolate* isolate = context->GetIsolate();

    const v8::PropertyAttribute kAttribute = static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
    TemplateFactory& tf = device_->template_factory();
    v8::Local<v8::Object> self = tf.NewService(isolate, this);
    // frontend http
    v8::Local<v8::Object> http = tf.NewHttpObject(isolate, this);
    http_.Reset(isolate, http);
    self->Set(
        v8::String::NewFromUtf8(isolate, "http")
        , http
        , kAttribute);
    http->Set(v8::String::NewFromUtf8(isolate, "service"), self, kAttribute);
    // backend channel
    v8::Local<v8::Object> channel = tf.NewChannel(isolate, this);
    self->Set(
        v8::String::NewFromUtf8(isolate, "channel")
        , channel
        , kAttribute);
    channel->Set(v8::String::NewFromUtf8(isolate, "service"), self, kAttribute);

    device_->FireServiceLoad(self);
    self_.Reset(isolate, self);
}

Service::~Service(void) {
    ServiceDesc::Delete(desc_);
}

void Service::RunShell(void) {
    v8::HandleScope handle_scope(device_->isolate());
    ::RunShell(device_->context());
}

// frontend http

int Service::HttpRequest(struct mg_connection* conn, enum mg_event ev) {

    if (ev == MG_REQUEST) {
        if (conn->is_websocket == 0) {
            if (device_->IsStaticURI(conn->uri)) {
                return device_->SendStaticFile(conn);
            }

            Http::Response res = req_manager_.HandleRequest(conn);
            res.Send(conn);
        } else {
            ws_manager_.HandleMessage(conn);
        }
        return MG_TRUE;
    }

    if (ev == MG_AUTH) {
        return MG_TRUE;
    }

    return MG_FALSE;
}

void Service::HttpNotify(const Http::Message& msg) {
    ws_manager_.DoNotify(msg);
}

v8::Local<v8::Object> Service::request_trigger(v8::Isolate* isolate) const {
    return req_manager_.request_trigger(isolate);
}

void Service::set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger) {
    req_manager_.set_request_trigger(isolate, trigger);
}

v8::Local<v8::Object> Service::open_trigger(v8::Isolate* isolate) const {
    return ws_manager_.open_trigger(isolate);
}

void Service::set_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    ws_manager_.set_open_trigger(isolate, trigger);
}

// backend channel

void Service::BindChannel(const ChannelRef& channel) {
    if (!channel_.IsNull()) {
        // event closed
    }
    if (!channel.IsNull()) {
        // event open
    }
    channel_ = channel;
}

void Service::UnbindChannel(void) {
    if (!channel_.IsNull()) {
        // event closed
    }
    channel_.Reset(0);
}

void Service::ChannelSend(const char* json) const {
    //channel_->Deliver();
}

v8::Local<v8::Object> Service::channel_open_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_channel_open_);
}

void Service::set_channel_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    on_channel_open_.Reset(isolate, trigger);
}

v8::Local<v8::Object> Service::channel_recv_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_channel_recv_);
}

void Service::set_channel_recv_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    on_channel_recv_.Reset(isolate, trigger);
}

v8::Local<v8::Object> Service::channel_closed_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_channel_closed_);
}

void Service::set_channel_closed_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    on_channel_closed_.Reset(isolate, trigger);
}

const char* Service::id(void) const {
    return desc_->id();
}



ServiceRef::ServiceRef(void)
    : impl_(0) {
    // nothing
}

ServiceRef::ServiceRef(const DeviceRef& device, ServiceDesc* desc)
    : impl_(new Service(device, desc)) {
    impl_->AddRef();
}

ServiceRef::ServiceRef(const ServiceRef& other)
    : impl_(0) {
    Reset(other.impl_);
}

ServiceRef::~ServiceRef(void) {
    if (impl_) {
        impl_->Release();
    }
}

ServiceRef& ServiceRef::operator= (const ServiceRef& other) {
    Reset(other.impl_);
    return *this;
}

bool ServiceRef::operator== (const ServiceRef& other) const {
    return (impl_ == other.impl_);
}

bool ServiceRef::operator!= (const ServiceRef& other) const {
    return (impl_ != other.impl_);
}

Service* ServiceRef::operator-> (void) const {
    return impl_;
}

Service* ServiceRef::Get(void) const {
    return impl_;
}

void ServiceRef::Reset(Service* service) {
    if (service) {
        service->AddRef();
    }
    if (impl_) {
        impl_->Release();
    }
    impl_ = service;
}

bool ServiceRef::IsNull(void) const {
    return (impl_ == 0);
}
