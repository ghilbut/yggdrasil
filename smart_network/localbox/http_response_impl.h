#ifndef HTTP_RESPONSE_IMPL_H_
#define HTTP_RESPONSE_IMPL_H_

#include <v8.h>
#include <boost/atomic.hpp>
#include <map>
#include <string>
#include <vector>


struct mg_connection;

namespace Http {

class Request;

class Response::Impl {
private:
    static void WeakCallback(const v8::WeakCallbackData<v8::Object, Response::Impl>& data);
    Impl(void);
    ~Impl(void) {}

public:
    static Impl* New(void);
    void AddRef(void);
    void Release(void);

    void MakeWeak(v8::Isolate* isolate, v8::Local<v8::Object> self);

    // methods
    void Send(struct mg_connection* conn) const;

    // properties
    int status_code(void) const;
    void set_status_code(int status_code);

    const char* GetHeader(const char* name) const;
    void SetHeader(const char* name, const char* value);
    void RemoveHeader(const char* name);

    const std::string& data(void) const;
    void set_data(const char* data, int data_len);


private:
    mutable boost::atomic_int ref_count_;

    typedef std::map<std::string, std::string> HeaderMap;
    int status_code_;
    HeaderMap headers_;
    std::string data_;

    v8::Persistent<v8::Object> self_;
};

}  // namespace Http

#endif  // HTTP_RESPONSE_IMPL_H_