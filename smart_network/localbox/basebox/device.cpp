#include "device.h"
#include "device_ref.h"

#include "sample.h"
#include "context.h"
#include "base_object/device_template.h"


Device::Device(const IOServiceRef& io_service, const char* basepath)
    : RefImplement()
    , context_(io_service, basepath)
    , template_factory_(context_.isolate())
    , storage_(basepath) {

    v8::Isolate* isolate = context_.isolate();
    v8::Local<v8::Context> context = context_.context();

    v8::Local<v8::Object> device = template_factory_.NewDevice(isolate, this);
    self_.Reset(isolate, device);
    this->AddRef();

    context->Global()->Set(v8::String::NewFromUtf8(isolate, "device"), device);




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
}

Device::~Device(void) {
    // nothing
}

void Device::FireServiceLoad(v8::Local<v8::Object> service) {
    v8::Isolate* isolate = context_.isolate(); 
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::New(isolate, on_service_load_);
    if (!obj.IsEmpty() && obj->IsFunction() && obj->IsCallable()) {
        v8::Local<v8::Object> caller = v8::Local<v8::Object>::New(isolate, self_);
        v8::Handle<v8::Value> params[1];
        params[0] = service;
        obj->CallAsFunction(caller, 1, params);
    }
}

v8::Isolate* Device::isolate(void) const {
    return context_.isolate();
}

v8::Handle<v8::Context> Device::context(void) const {
    return context_.context();
}

TemplateFactory& Device::template_factory(void) const {
    return template_factory_;
}

Storage& Device::storage(void) const {
    return storage_;
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

v8::Local<v8::Object> Device::closed_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_closed_);
}

void Device::set_closed_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    on_closed_.Reset(isolate, trigger);
}



DeviceRef::DeviceRef(void)
    : impl_(0) {
    // nothing
}

DeviceRef::DeviceRef(const IOServiceRef& io_service, const char* basepath) 
    : impl_(new Device(io_service, basepath)) {
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

void DeviceRef::Reset(Device* device) {
    if (device) {
        device->AddRef();
    }
    if (impl_) {
        impl_->Release();
    }
    impl_ = device;
}

bool DeviceRef::IsNull(void) const {
    return (impl_ == 0);
}
