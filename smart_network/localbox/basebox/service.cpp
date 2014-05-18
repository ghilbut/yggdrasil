#include "service.h"
#include "service_ref.h"

#include "environ.h"


Service::Service(Environ& env)
    : RefImplement()
    , env_(env) {
    
    v8::Isolate* isolate = env_.isolate();
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    TemplateFactory& tf = env_.template_factory();
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

ServiceRef::ServiceRef(Environ& env)
    : impl_(new Service(env)) {
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