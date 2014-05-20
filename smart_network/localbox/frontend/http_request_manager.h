#ifndef FRONT_END_HTTP_REQUEST_MANAGER_H_
#define FRONT_END_HTTP_REQUEST_MANAGER_H_

#include "http_response.h"
#include <v8.h>
#include <boost/thread.hpp>


class DeviceContext;
struct mg_connection;

namespace Http {

class RequestManager {
public:
    RequestManager(DeviceContext* context, v8::Persistent<v8::Object>& caller);
    ~RequestManager(void);

    Response HandleRequest(struct mg_connection *conn);

    v8::Local<v8::Object> request_trigger(v8::Isolate* isolate) const;
    void set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger);

private:
    void event_request(struct mg_connection *conn, boost::function<void(const Response&)> ret_setter);


private:
    DeviceContext* context_;

    v8::Persistent<v8::Object>& caller_;
    v8::Persistent<v8::Object> on_request_;
};

}  // namespace Http

#endif  // FRONT_END_HTTP_REQUEST_MANAGER_H_