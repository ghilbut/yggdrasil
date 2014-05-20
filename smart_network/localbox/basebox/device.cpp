#include "device.h"
#include "device_ref.h"

#include "environ.h"


/*DeviceHost::DeviceHost(IOServiceRef io_service, const char* basepath)
    : RefImplement()
    , context_(io_service, basepath)
    , template_factory_(context_.isolate()) {
        // nothing
}*/

DeviceHost::DeviceHost(DeviceContext& context)
    : RefImplement()
    , context_(context) {
    
    v8::Isolate* isolate = context_.isolate();
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    TemplateFactory& tf = context_.template_factory();
    self_.Reset(isolate, tf.NewDevice(isolate, this));
    this->AddRef();
}

DeviceHost::~DeviceHost(void) {
    // nothing
}

void DeviceHost::FireServiceLoaded() {
    v8::Isolate* isolate = context_.isolate(); 
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

void DeviceHost::FireChannelOpend() {
    v8::Isolate* isolate = context_.isolate(); 
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

v8::Isolate* DeviceHost::isolate(void) const {
    return context_.isolate();
}

TemplateFactory& DeviceHost::template_factory(void) const {
    //return template_factory_;
    return context_.template_factory();
}

v8::Local<v8::Object> DeviceHost::self(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, self_);
}

v8::Local<v8::Object> DeviceHost::service_load_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_service_load_);
}

void DeviceHost::set_service_load_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    on_service_load_.Reset(isolate, trigger);
}

v8::Local<v8::Object> DeviceHost::channel_open_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_channel_open_);
}

void DeviceHost::set_channel_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    on_channel_open_.Reset(isolate, trigger);
}





DeviceRef::DeviceRef(void)
    : impl_(0) {
    // nothing
}

DeviceRef::DeviceRef(DeviceContext& context)
    : impl_(new DeviceHost(context)) {
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

void DeviceRef::Reset(DeviceHost* device) {
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

DeviceHost* DeviceRef::operator-> (void) const {
    return impl_;
}