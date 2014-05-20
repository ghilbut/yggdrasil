#include "http_websocket.h"
#include "http_websocket_impl.h"
#include "http_websocket_template.h"
#include "http_message.h"
#include "basebox/device.h"
#include "basebox/template_factory.h"
#include <mongoose.h>


namespace Http {

WebSocket::WebSocket(void)
    : pimpl_(0) {
    // nothing
}

WebSocket::WebSocket(const DeviceRef& device, struct mg_connection* conn)
    : pimpl_(Impl::New(device, conn)) {
    if (pimpl_) {
        pimpl_->AddRef();
    }
}

WebSocket::WebSocket(const WebSocket& other)
    : pimpl_(other.pimpl_) {
    if (pimpl_) {
        pimpl_->AddRef();
    }
}

WebSocket::~WebSocket(void) {
    if (pimpl_) {
        pimpl_->Release();
    }
}

WebSocket& WebSocket::operator= (WebSocket& other) {
    if (other.pimpl_) {
        other.pimpl_->AddRef();
    }
    if (pimpl_) {
        pimpl_->Release();
    }
    pimpl_ = other.pimpl_;
    return *this;
}

void WebSocket::DoSend(const Message& msg) const {
    if (pimpl_) {
        pimpl_->DoSend(msg);
    }
}

void WebSocket::FireMessage(const Message& msg) const {
    if (pimpl_) {
        pimpl_->FireMessage(msg);
    }
}

void WebSocket::FireClosed(void) const {
    if (pimpl_) {
        pimpl_->FireClosed();
    }
}

v8::Local<v8::Object> WebSocket::self(v8::Isolate* isolate) const {
    return (pimpl_ ? pimpl_->self(isolate) : v8::Undefined(isolate)->ToObject());
}

v8::Local<v8::Object> WebSocket::message_trigger(v8::Isolate* isolate) const {
    return (pimpl_ ? pimpl_->message_trigger(isolate) : v8::Undefined(isolate)->ToObject());
}
void WebSocket::set_message_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    if (pimpl_) {
        pimpl_->set_message_trigger(isolate, trigger);
    }
}

v8::Local<v8::Object> WebSocket::closed_trigger(v8::Isolate* isolate) const {
    return (pimpl_ ? pimpl_->closed_trigger(isolate) : v8::Undefined(isolate)->ToObject());
}
void WebSocket::set_closed_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    if (pimpl_) {
        pimpl_->set_closed_trigger(isolate, trigger);
    }
}

WebSocket::Impl::Impl(const DeviceRef& device, struct mg_connection* conn)
    : RefImplement()
    , device_(device)
    , conn_(conn) {

    v8::Isolate* isolate = device_->isolate();
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    TemplateFactory& tf = device_->template_factory();
    v8::Local<v8::Object> self = tf.NewHttpWebSocket(isolate, this);
    this->AddRef();

    self_.Reset(isolate, self);
    //self_.MarkIndependent();
    //self_.SetWeak(this, &WebSocket::Impl::WeakCallback);
}

WebSocket::Impl::~Impl(void) {
    // nothing
}

WebSocket::Impl* WebSocket::Impl::New(const DeviceRef& device, struct mg_connection* conn) {
    return new Impl(device, conn);
}

void WebSocket::Impl::WeakCallback(const v8::WeakCallbackData<v8::Object, WebSocket::Impl>& data) {
    Impl* pThis = data.GetParameter();
    (pThis->self_).Reset();
    pThis->Release();
}

void WebSocket::Impl::DoSend(const Message& msg) const {
    mg_websocket_write(conn_, 0x1, msg.data(), msg.data_len());
}

void WebSocket::Impl::FireMessage(const Message& msg) const {
    v8::Isolate* isolate = device_->isolate(); 
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::New(isolate, on_message_);
    if (!obj.IsEmpty() && obj->IsFunction() && obj->IsCallable()) {
        v8::Local<v8::Object> caller = v8::Local<v8::Object>::New(isolate, self_);
        v8::Handle<v8::Value> params[1];
        params[0] = v8::String::NewFromUtf8(isolate, msg.data(), v8::String::kNormalString, msg.data_len());
        obj->CallAsFunction(caller, 1, params);
    }
}

void WebSocket::Impl::FireClosed(void) {
    v8::Isolate* isolate = device_->isolate(); 
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::New(isolate, on_closed_);
    if (!obj.IsEmpty() && obj->IsFunction() && obj->IsCallable()) {
        v8::Local<v8::Object> caller = v8::Local<v8::Object>::New(isolate, self_);
        obj->CallAsFunction(caller, 0, 0);
    }

    self_.SetWeak(this, &WebSocket::Impl::WeakCallback);
}

v8::Local<v8::Object> WebSocket::Impl::self(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, self_);
}

v8::Local<v8::Object> WebSocket::Impl::message_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_message_);
}
void WebSocket::Impl::set_message_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    on_message_.Reset(device_->isolate(), trigger);
}

v8::Local<v8::Object> WebSocket::Impl::closed_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_closed_);
}
void WebSocket::Impl::set_closed_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    on_closed_.Reset(device_->isolate(), trigger);
}

}  // namespace Http
