#ifndef FRONT_END_HTTP_OBJECT_TEMPLATE_H_
#define FRONT_END_HTTP_OBJECT_TEMPLATE_H_

#include <v8.h>


namespace Http {

class ObjectTemplate {
public:
    static v8::Local<v8::FunctionTemplate> New(v8::Isolate* isolate
                                               , v8::Local<v8::FunctionTemplate> response);

private:
    static void GetEventRequest(v8::Local<v8::String> property
                                , const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEventRequest(v8::Local<v8::String> property
                                , v8::Local<v8::Value> value
                                , const v8::PropertyCallbackInfo<void>& info);
    static void GetEventWebSocket(v8::Local<v8::String> property
                                  , const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEventWebSocket(v8::Local<v8::String> property
                                  , v8::Local<v8::Value> value
                                  , const v8::PropertyCallbackInfo<void>& info);

    static void Notify(const v8::FunctionCallbackInfo<v8::Value>& args);

private:
    ObjectTemplate(void) {}
};

}  // namespace Http

#endif  // FRONT_END_HTTP_OBJECT_TEMPLATE_H_