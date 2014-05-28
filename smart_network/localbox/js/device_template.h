#ifndef BASE_OBJECT_DEVICE_TEMPLATE_H_
#define BASE_OBJECT_DEVICE_TEMPLATE_H_

#include <v8.h>


class DeviceTemplate {
public:
    static v8::Local<v8::FunctionTemplate> New(v8::Isolate* isolate);

private:
    static void GetEventServiceLoaded(v8::Local<v8::String> property
        , const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEventServiceLoaded(v8::Local<v8::String> property
        , v8::Local<v8::Value> value
        , const v8::PropertyCallbackInfo<void>& info);
    static void GetEventClosed(v8::Local<v8::String> property
        , const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEventClosed(v8::Local<v8::String> property
        , v8::Local<v8::Value> value
        , const v8::PropertyCallbackInfo<void>& info);


private:
    DeviceTemplate(void) {}
};

#endif  // BASE_OBJECT_DEVICE_TEMPLATE_H_