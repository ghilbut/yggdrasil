#include "device.h"
#include "device_ref.h"

#include "environ.h"


Device::Device(Environ& env)
    : RefImplement()
    , env_(env) {
    
    v8::Isolate* isolate = env_.isolate();
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    TemplateFactory& tf = env_.template_factory();
    self_.Reset(isolate, tf.NewDevice(isolate, this));
    this->AddRef();
}

Device::~Device(void) {
    // nothing
}

void Device::FireServiceLoaded() {
    v8::Isolate* isolate = env_.isolate(); 
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::New(isolate, on_service_load_);
    if (!obj.IsEmpty() && obj->IsFunction() && obj->IsCallable()) {
        v8::Local<v8::Object> caller = v8::Local<v8::Object>::New(isolate, self_);
        //v8::Handle<v8::Value> params[1];
        //params[0] = v8::String::NewFromUtf8(isolate, msg.data(), v8::String::kNormalString, msg.data_len());
        //obj->CallAsFunction(caller, 1, params);
    }
}

void Device::FireChannelOpend() {
    v8::Isolate* isolate = env_.isolate(); 
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::New(isolate, on_channel_open_);
    if (!obj.IsEmpty() && obj->IsFunction() && obj->IsCallable()) {
        v8::Local<v8::Object> caller = v8::Local<v8::Object>::New(isolate, self_);
        //v8::Handle<v8::Value> params[1];
        //params[0] = v8::String::NewFromUtf8(isolate, msg.data(), v8::String::kNormalString, msg.data_len());
        //obj->CallAsFunction(caller, 1, params);
    }
}

v8::Local<v8::Object> Device::self(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, self_);
}

v8::Local<v8::Object> Device::service_load_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_service_load_);
}

void Device::set_service_load_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    on_service_load_.Reset(isolate, trigger);
}

v8::Local<v8::Object> Device::channel_open_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_channel_open_);
}

void Device::set_channel_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    on_channel_open_.Reset(isolate, trigger);
}





DeviceRef::DeviceRef(void)
    : impl_(0) {
    // nothing
}

DeviceRef::DeviceRef(Environ& env)
    : impl_(new Device(env)) {
    impl_->AddRef();
}

DeviceRef::DeviceRef(const DeviceRef& other)
    : impl_(0) {
    Reset(other.impl_);
}

DeviceRef::~DeviceRef(void) {
    if (impl_) {
        impl_->Release();
    }
}

void DeviceRef::Reset(Device* device) {
    if (device) {
        device->AddRef();
    }
    if (impl_) {
        impl_->Release();
    }
    impl_ = device;
}

DeviceRef& DeviceRef::operator= (const DeviceRef& other) {
    Reset(other.impl_);
    return *this;
}

bool DeviceRef::operator== (const DeviceRef& other) const {
    return (impl_ == other.impl_);
}

bool DeviceRef::operator!= (const DeviceRef& other) const {
    return (impl_ != other.impl_);
}

Device* DeviceRef::operator-> (void) const {
    return impl_;
}