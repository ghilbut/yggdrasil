#ifndef HTTP_WEBSOCKET_TEMPLATE_H_
#define HTTP_WEBSOCKET_TEMPLATE_H_

#include <v8.h>


class Environ;

namespace Http {

class WebSocket;

class WebSocketTemplate {
public:
    static v8::Local<v8::FunctionTemplate> Get(v8::Isolate* isolate);
    static v8::Local<v8::Object> NewInstance(v8::Isolate* isolate, struct mg_connection* conn);

    template<typename T>
    static WebSocket* Unwrap(T _t);
    static void WebSocketTemplate::Send(const v8::FunctionCallbackInfo<v8::Value>& args);

private:
    WebSocketTemplate(void) {}
};

}  // namespace Http

#endif  // HTTP_WEBSOCKET_TEMPLATE_H_