#include "http_response.h"
#include "http_response_impl.h"
#include "http_request.h"
#include <mongoose.h>
#include <cassert>


namespace Http {

Response::Response(void)
    : pimpl_(0) {
    // nothing
}

Response::Response(const Response& other)
    : pimpl_(other.pimpl_) {
    if (pimpl_) {
        pimpl_->AddRef();
    }
}

Response::~Response(void) {
    if (pimpl_) {
        pimpl_->Release();
    }
}

void Response::Reset(Response::Impl* pimpl) {
    if (pimpl) {
        pimpl->AddRef();
    }
    if (pimpl_) {
        pimpl_->Release();
    }
    pimpl_ = pimpl;
}

Response& Response::operator= (const Response& other) {
    if (other.pimpl_) {
        other.pimpl_->AddRef();
    }
    if (pimpl_) {
        pimpl_->Release();
    }
    pimpl_ = other.pimpl_;
    return *this;
}

int Response::status_code(void) const {
    return pimpl_->status_code();
}

void Response::set_status_code(int status_code) {
    pimpl_->set_status_code(status_code);
}

const char* Response::GetHeader(const char* name) const {
    return pimpl_->GetHeader(name);
}

void Response::SetHeader(const char* name, const char* value) {
    pimpl_->SetHeader(name, value);
}

void Response::RemoveHeader(const char* name) {
    pimpl_->RemoveHeader(name);
}

const std::string& Response::data(void) const {
    return pimpl_->data();
}

void Response::set_data(const char* data, int data_len) {
    pimpl_->set_data(data, data_len);
}

void Response::Send(struct mg_connection* conn) const {
    pimpl_->Send(conn);
}

Response::Impl::Impl(void) 
    : RefObject()
    , status_code_(200) {
    // nothing
}

Response::Impl* Response::Impl::New(void) {
    return new Impl();
}

void Response::Impl::WeakCallback(const v8::WeakCallbackData<v8::Object, Response::Impl>& data) {
    Response::Impl* pThis = data.GetParameter();
    (pThis->self_).Reset();
    pThis->Release();
}

void Response::Impl::MakeWeak(v8::Isolate* isolate, v8::Local<v8::Object> self) {
    v8::HandleScope handle_scope(isolate);
    self_.Reset(isolate, self);
    self->SetAlignedPointerInInternalField(0, this);
    this->AddRef();
    //self_.MarkIndependent();
    self_.SetWeak(this, WeakCallback);
}

int Response::Impl::status_code(void) const {
    return status_code_;
}

void Response::Impl::set_status_code(int status_code) {
    status_code_ = status_code;
}

const char* Response::Impl::GetHeader(const char* name) const {
    HeaderMap::const_iterator itr = headers_.find(name);
    if (itr == headers_.end()) {
        return 0;
    }
    return (itr->second).c_str();
}

void Response::Impl::SetHeader(const char* name, const char* value) {
    headers_[name] = value;
}

void Response::Impl::RemoveHeader(const char* name) {
    HeaderMap::const_iterator itr = headers_.find(name);
    if (itr != headers_.end()) {
        headers_.erase(itr);
    }
}

const std::string& Response::Impl::data(void) const {
    return data_;
}

void Response::Impl::set_data(const char* data, int data_len) {
    data_.swap(std::string(data, data + data_len));
}

void Response::Impl::Send(struct mg_connection* conn) const {

    mg_send_status(conn, status_code_);

    if (!headers_.empty()) {
        HeaderMap::const_iterator itr = headers_.begin();
        HeaderMap::const_iterator end = headers_.end();
        for (; itr != end; ++itr) {
            const std::string& name = itr->first;
            const std::string& val = itr->second;
            if (!name.empty() && !val.empty()) {
                mg_send_header(conn, name.c_str(), val.c_str());
            }
        }
    }

    if (data_.empty()) {
        mg_send_data(conn, 0, 0);
    } else {
        mg_send_data(conn, data_.c_str(), data_.length());
    }
}

}  // namespace Http