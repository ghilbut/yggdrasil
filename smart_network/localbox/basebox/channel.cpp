#include "channel.h"
#include "channel_ref.h"

#include "device.h"
#include "template_factory.h"


Channel::Channel(Device& device)
    : RefObject()
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







ChannelRef::ChannelRef(Device& device)
    : impl_(new Channel(device)) {
    impl_->AddRef();
}
