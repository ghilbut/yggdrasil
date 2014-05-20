#include "channel.h"
#include "channel_ref.h"

#include "environ.h"


Channel::Channel(DeviceContext& context)
    : RefImplement()
    , context_(context) {
    
    v8::Isolate* isolate = context_.isolate();
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    TemplateFactory& tf = context_.template_factory();
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

ChannelRef::ChannelRef(DeviceContext& context)
    : impl_(new Channel(context)) {
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