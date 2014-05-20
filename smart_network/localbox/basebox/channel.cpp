#include "channel.h"
#include "channel_ref.h"

#include "device_host.h"
#include "template_factory.h"


Channel::Channel(DeviceHost& device)
    : RefImplement()
    , device_(device) {
    
    v8::Isolate* isolate = device_.isolate();
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    TemplateFactory& tf = device_.template_factory();
    self_.Reset(isolate, tf.NewChannel(isolate, this));
    this->AddRef();
}

Channel::~Channel(void) {
    // nothing
}

v8::Local<v8::Object> Channel::self(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, self_);
}






ChannelRef::ChannelRef(void)
    : impl_(0) {
    // nothing
}

ChannelRef::ChannelRef(DeviceHost& device)
    : impl_(new Channel(device)) {
    impl_->AddRef();
}

ChannelRef::ChannelRef(const ChannelRef& other)
    : impl_(0) {
    Reset(other.impl_);
}

ChannelRef::~ChannelRef(void) {
    if (impl_) {
        impl_->Release();
    }
}

void ChannelRef::Reset(Channel* channel) {
    if (channel) {
        channel->AddRef();
    }
    if (impl_) {
        impl_->Release();
    }
    impl_ = channel;
}

ChannelRef& ChannelRef::operator= (const ChannelRef& other) {
    Reset(other.impl_);
    return *this;
}

bool ChannelRef::operator== (const ChannelRef& other) const {
    return (impl_ == other.impl_);
}

bool ChannelRef::operator!= (const ChannelRef& other) const {
    return (impl_ != other.impl_);
}

Channel* ChannelRef::operator-> (void) const {
    return impl_;
}