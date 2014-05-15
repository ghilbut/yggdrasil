#include "http_message.h"
#include "http_message_impl.h"


namespace Http {

Message::Message(void)
    : pimpl_(0) {
    // nothing
}

Message::Message(const char* data, size_t data_len)
    : pimpl_(new Impl(data, data_len)) {
    // nothing
}

Message::Message(const Message& other)
    : pimpl_(other.pimpl_) {
    if (pimpl_) {
        pimpl_->AddRef();
    }
}

Message::~Message(void) {
    if (pimpl_) {
        pimpl_->Release();
    }
}

Message& Message::operator= (Message& other) {
    if (other.pimpl_) {
        other.pimpl_->AddRef();
    }
    if (pimpl_) {
        pimpl_->Release();
    }
    pimpl_ = other.pimpl_;
    return *this;
}

const char* Message::data(void) const {
    return (pimpl_ ? pimpl_->data() : 0);
}

size_t Message::data_len(void) const {
    return (pimpl_ ? pimpl_->data_len() : 0);
}

Message::Impl::Impl(const char* data, size_t data_len)
    : RefImplement(1)
    , data_(data) 
    , data_len_(data_len) {
    // nothing
}

Message::Impl::~Impl(void) {
    if (data_) {
        delete[] data_;
    }
}

const char* Message::Impl::data(void) const {
    return data_;
}

size_t Message::Impl::data_len(void) const {
    return data_len_;
}

}  // namespace Http
