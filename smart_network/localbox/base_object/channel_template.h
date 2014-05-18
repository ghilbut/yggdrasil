#ifndef BASE_OBJECT_CHANNEL_TEMPLATE_H_
#define BASE_OBJECT_CHANNEL_TEMPLATE_H_

#include <v8.h>


class ChannelTemplate {
public:
    static v8::Local<v8::FunctionTemplate> New(v8::Isolate* isolate);

private:
    static void GetEventRecv(v8::Local<v8::String> property
        , const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEventRecv(v8::Local<v8::String> property
        , v8::Local<v8::Value> value
        , const v8::PropertyCallbackInfo<void>& info);
    static void GetEventClosed(v8::Local<v8::String> property
        , const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEventClosed(v8::Local<v8::String> property
        , v8::Local<v8::Value> value
        , const v8::PropertyCallbackInfo<void>& info);

    static void Send(const v8::FunctionCallbackInfo<v8::Value>& args);

private:
    ChannelTemplate(void) {}
};

#endif  // BASE_OBJECT_CHANNEL_TEMPLATE_H_