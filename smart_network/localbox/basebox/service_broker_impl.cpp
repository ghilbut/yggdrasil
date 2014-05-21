#include "service_broker_impl.h"

#include "base_object/file_object.h"
#include "frontend/http_object_template.h"
#include "frontend/http_request.h"
#include "storage.h"
#include "sample.h"
#include "http.h"
#include "template_factory.h"
#include "http_server_template.h"
#include "basebox/device.h"
#include <mongoose.h>


ServiceBroker::ServiceBroker(const DeviceRef& device_ref)
    : pimpl_(new ServiceBroker::Impl(device_ref)) {
    // nothing
}

ServiceBroker::~ServiceBroker(void) {
}

void ServiceBroker::RunShell(void) {
    pimpl_->RunShell();
}

int ServiceBroker::HttpRequest(struct mg_connection* conn, enum mg_event ev) {
    return pimpl_->HttpRequest(conn, ev);
}

void ServiceBroker::HttpNotify(const Http::Message& msg) {
    pimpl_->HttpNotify(msg);
}

v8::Local<v8::Object> ServiceBroker::request_trigger(v8::Isolate* isolate) const {
    return pimpl_->request_trigger(isolate);
}

void ServiceBroker::set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger) {
    pimpl_->set_request_trigger(isolate, trigger);
}

v8::Local<v8::Object> ServiceBroker::open_trigger(v8::Isolate* isolate) const {
    return pimpl_->open_trigger(isolate);
}

void ServiceBroker::set_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger) {
    pimpl_->set_open_trigger(isolate, trigger);
}

ServiceBroker::Impl::Impl(const DeviceRef& device_ref)
    : device_(device_ref)
    , storage_(device_->storage())
    , req_manager_(device_, http_)
    , ws_manager_(device_, self_, http_) {

    v8::Local<v8::Context> context = device_->context();
    v8::Isolate* isolate = context->GetIsolate();



    BindSample(context);




    const v8::PropertyAttribute kAttribute = static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
    TemplateFactory& tf = device_->template_factory();

    /*v8::Local<v8::Object> http = tf.NewHttpObject(isolate, this);
    http_.Reset(isolate, http);
    context->Global()->Set(
        v8::String::NewFromUtf8(isolate, "http")
        , http
        , kAttribute);*/





    v8::Handle<v8::String> source = ReadFile(isolate, storage_.settings());
    if (source.IsEmpty()) {
        //return -1;
    }
    if (!ExecuteString(isolate,
        source,
        v8::String::NewFromUtf8(isolate, storage_.settings()),
        true, //false,
        true)) { //false)) {
            //return -1;
    }


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

ServiceBroker::Impl::~Impl(void) {
    // nothing
}

void ServiceBroker::Impl::RunShell(void) {
    v8::HandleScope handle_scope(device_->isolate());
    ::RunShell(device_->context());
}



int ServiceBroker::Impl::HttpRequest(struct mg_connection* conn, enum mg_event ev) {

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

void ServiceBroker::Impl::HttpNotify(const Http::Message& msg) {
    ws_manager_.DoNotify(msg);
}

v8::Local<v8::Object> ServiceBroker::Impl::request_trigger(v8::Isolate* isolate) const {
    return req_manager_.request_trigger(isolate);
}

void ServiceBroker::Impl::set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger) {
    req_manager_.set_request_trigger(isolate, trigger);
}

v8::Local<v8::Object> ServiceBroker::Impl::open_trigger(v8::Isolate* isolate) const {
    return ws_manager_.open_trigger(isolate);
}

void ServiceBroker::Impl::set_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    ws_manager_.set_open_trigger(isolate, trigger);
}
