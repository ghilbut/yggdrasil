#include "device_template.h"

#include "basebox/device.h"
//#include "basebox/service_broker.h"
//#include "basebox/service_broker_impl.h"


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
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onchannel")
            , &DeviceTemplate::GetEventChannelOpened
            , &DeviceTemplate::SetEventChannelOpened);
        ot->Set(isolate, "notify"
            , v8::FunctionTemplate::New(isolate, &DeviceTemplate::Notify));

        return ft;
}

void DeviceTemplate::GetEventServiceLoaded(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    //Device* d = Unwrap(info);
    //info.GetReturnValue().Set(d->request_trigger(info.GetIsolate()));
}

void DeviceTemplate::SetEventServiceLoaded(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    //Unwrap(info)->set_request_trigger(info.GetIsolate(), value->ToObject());
}

void DeviceTemplate::GetEventChannelOpened(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    //ServiceBroker::Impl* s = Unwrap(info);
    //info.GetReturnValue().Set(s->request_trigger(info.GetIsolate()));
}

void DeviceTemplate::SetEventChannelOpened(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    //Unwrap(info)->set_request_trigger(info.GetIsolate(), value->ToObject());
}

void DeviceTemplate::Notify(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);

    if (args.Length() < 1) {
        // TODO(ghilbut): throw exception
        return;
    }

    if (!args[0]->IsString()) {
        // TODO(ghilbut): throw exception
        return;
    }

    v8::Local<v8::String> str = args[0]->ToString();

    //const int data_len = str->Utf8Length();
    //char* data = new char[data_len];
    //str->WriteUtf8(data, data_len);

    //const Message msg(data, data_len);
    //Unwrap(args)->DeviceNotify(msg);
}
