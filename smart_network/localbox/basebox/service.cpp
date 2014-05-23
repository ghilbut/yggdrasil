#include "service.h"
#include "service_ref.h"

#include "frontend/http_object_template.h"
#include "frontend/http_request.h"
#include "sample.h"
#include "template_factory.h"
#include "basebox/device.h"
#include <mongoose.h>



Service::Service(const DeviceRef& device_ref)
    : device_(device_ref)
    , storage_(device_->storage())
    , req_manager_(device_, http_)
    , ws_manager_(device_, self_, http_) {

    v8::Local<v8::Context> context = device_->context();
    v8::Isolate* isolate = context->GetIsolate();

    const v8::PropertyAttribute kAttribute = static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
    TemplateFactory& tf = device_->template_factory();
    v8::Local<v8::Object> self = tf.NewService(isolate, this);
    v8::Local<v8::Object> http = tf.NewHttpObject(isolate, this);
    http_.Reset(isolate, http);
    self->Set(
        v8::String::NewFromUtf8(isolate, "http")
        , http
        , kAttribute);
    http->Set(v8::String::NewFromUtf8(isolate, "service"), self, kAttribute);

    device_->FireServiceLoad(self);
    self_.Reset(isolate, self);
}

Service::~Service(void) {
    // nothing
}

void Service::RunShell(void) {
    v8::HandleScope handle_scope(device_->isolate());
    ::RunShell(device_->context());
}



int Service::HttpRequest(struct mg_connection* conn, enum mg_event ev) {

    if (ev == MG_REQUEST) {
        if (conn->is_websocket == 0) {
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



ServiceRef::ServiceRef(void)
    : impl_(0) {
    // nothing
}

ServiceRef::ServiceRef(const DeviceRef& device)
    : impl_(new Service(device)) {
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
