#include "channel.h"
#include "channel_ref.h"

//#include "backend/channel.h"
#include "backend/channel_ref.h"
#include <cassert>


Channel::Channel(const IOServiceRef& io_service) 
    : io_service_(io_service)
    , delegate_(0) {
    // nothing
}

void Channel::BindDelegate(ChannelDelegate* delegate) {
    assert(delegate != 0);
    delegate_ = delegate;
}

void Channel::UnbindDelegate(void) {
    assert(delegate_ != 0);
    delegate_ = 0;
}

void Channel::FireEventOpen(const std::string& text) {
    assert(delegate_ != 0);
    if (delegate_) {
        delegate_->OnChannelOpen(ChannelRef(this), text);
    }
}

void Channel::FireEventRecv(const std::string& text) {
    assert(delegate_ != 0);
    if (delegate_) {
        delegate_->OnChannelRecv(ChannelRef(this), text);
    }
}

void Channel::FireEventClosed(void) {
    assert(delegate_ != 0);
    if (delegate_) {
        delegate_->OnChannelClosed(ChannelRef(this));
    }
}



ChannelRef::ChannelRef(void)
    : impl_(0) {
    // nothing
}

ChannelRef::ChannelRef(const ChannelRef& other)
    : impl_(other.impl_) {
    if (impl_) {
        impl_->AddRef();
    }
}

ChannelRef::ChannelRef(Channel* channel)
    : impl_(channel) {
    if (impl_) {
        impl_->AddRef();
    }
}

//ChannelRef::ChannelRef(Device& device) {
//}

ChannelRef::~ChannelRef(void) {
    Reset(0);
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

Channel& ChannelRef::operator* (void) const {
    return *impl_;
}

Channel* ChannelRef::operator-> (void) const {
    return impl_;
}

Channel* ChannelRef::Get(void) const {
    return impl_;
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

bool ChannelRef::IsNull(void) const {
    return (impl_ == 0);
}
