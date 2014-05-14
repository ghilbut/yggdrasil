#ifndef HTTP_WEBSOCKET_TEMPLATE_H_
#define HTTP_WEBSOCKET_TEMPLATE_H_

#include <v8.h>


class Environ;

namespace Http {

class WebSocketTemplate {
public:
    static v8::Local<v8::FunctionTemplate> Get(v8::Isolate* isolate);

    static void GetEventMessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEventMessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetEventClosed(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEventClosed(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

    static void Send(const v8::FunctionCallbackInfo<v8::Value>& args);

private:
    WebSocketTemplate(void) {}
};

}  // namespace Http

#endif  // HTTP_WEBSOCKET_TEMPLATE_H_