#include "device_template.h"

#include "basebox/device.h"


template<typename T>
static Device* Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    //v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    //void* ptr = wrap->Value();
    //return static_cast<ServiceBroker*>(ptr);
    return static_cast<Device*>(object->GetAlignedPointerFromInternalField(0));
}

v8::Local<v8::FunctionTemplate> DeviceTemplate::New(v8::Isolate* isolate) {

        v8::Local<v8::FunctionTemplate> ft = v8::FunctionTemplate::New(isolate);
        ft->SetClassName(v8::String::NewFromUtf8(isolate, "Device"));

        v8::Local<v8::ObjectTemplate> ot = ft->InstanceTemplate();
        ot->SetInternalFieldCount(1);
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onservice")
            , &DeviceTemplate::GetEventServiceLoaded
            , &DeviceTemplate::SetEventServiceLoaded);
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onclosed")
            , &DeviceTemplate::GetEventClosed
            , &DeviceTemplate::SetEventClosed);

        return ft;
}

void DeviceTemplate::GetEventServiceLoaded(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* const isolate = info.GetIsolate();
    info.GetReturnValue().Set(Unwrap(info)->service_load_trigger(isolate));
}

void DeviceTemplate::SetEventServiceLoaded(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* const isolate = info.GetIsolate();
    Unwrap(info)->set_service_load_trigger(isolate, value->ToObject());
}

void DeviceTemplate::GetEventClosed(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* const isolate = info.GetIsolate();
    info.GetReturnValue().Set(Unwrap(info)->closed_trigger(isolate));
}

void DeviceTemplate::SetEventClosed(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* const isolate = info.GetIsolate();
    Unwrap(info)->set_closed_trigger(isolate, value->ToObject());
}
