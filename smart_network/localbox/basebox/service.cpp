#include "service.h"
#include "service_ref.h"

#include "device.h"
#include "template_factory.h"


Service::Service(Device& device)
    : RefImplement()
    , device_(device) {
    
    v8::Isolate* isolate = device_.isolate();
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    TemplateFactory& tf = device_.template_factory();
    self_.Reset(isolate, tf.NewDevice(isolate, this));
    this->AddRef();
}

Service::~Service(void) {
    // nothing
}





ServiceRef::ServiceRef(void)
    : impl_(0) {
    // nothing
}

ServiceRef::ServiceRef(Device& device)
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

void ServiceRef::Reset(Service* service) {
    if (service) {
        service->AddRef();
    }
    if (impl_) {
        impl_->Release();
    }
    impl_ = service;
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