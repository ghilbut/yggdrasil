#include "device_host.h"
#include "device_host_ref.h"

#include "device_context.h"
#include "base_object/device_template.h"


DeviceHost::DeviceHost(const IOServiceRef& io_service, const char* basepath)
    : RefImplement()
    , context_(io_service, basepath) {
    //, template_factory_(context_.isolate()) {

    v8::Isolate* isolate = context_.isolate();
    self_.Reset(isolate, context_.template_factory().NewDevice(isolate, this));
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

/*template <typename F>
void DeviceHost::Post(const F& handler) {
    context_.Post(handler);
}*/

v8::Isolate* DeviceHost::isolate(void) const {
    return context_.isolate();
}

v8::Handle<v8::Context> DeviceHost::context(void) const {
    return context_.context();
}

TemplateFactory& DeviceHost::template_factory(void) const {
    //return template_factory_;
    return context_.template_factory();
}

Storage& DeviceHost::storage(void) const {
    return context_.storage();
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

DeviceRef::DeviceRef(const IOServiceRef& io_service, const char* basepath) 
    : impl_(new DeviceHost(io_service, basepath)) {
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