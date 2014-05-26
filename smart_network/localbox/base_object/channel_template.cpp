#include "channel_template.h"

#include "basebox/service.h"


template<typename T>
static Service* Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    //v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    //void* ptr = wrap->Value();
    //return static_cast<ServiceBroker*>(ptr);
    return static_cast<Service*>(object->GetAlignedPointerFromInternalField(0));
}

v8::Local<v8::FunctionTemplate> ChannelTemplate::New(v8::Isolate* isolate) {

        v8::Local<v8::FunctionTemplate> ft = v8::FunctionTemplate::New(isolate);
        ft->SetClassName(v8::String::NewFromUtf8(isolate, "Device"));

        v8::Local<v8::ObjectTemplate> ot = ft->InstanceTemplate();
        ot->SetInternalFieldCount(1);
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onopen")
            , &ChannelTemplate::GetEventOpen
            , &ChannelTemplate::SetEventOpen);
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onrecv")
            , &ChannelTemplate::GetEventRecv
            , &ChannelTemplate::SetEventRecv);
        ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onclosed")
            , &ChannelTemplate::GetEventClosed
            , &ChannelTemplate::SetEventClosed);
        ot->Set(isolate, "send"
            , v8::FunctionTemplate::New(isolate, &ChannelTemplate::Send));

        return ft;
}

void ChannelTemplate::GetEventOpen(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    info.GetReturnValue().Set(Unwrap(info)->channel_open_trigger(isolate));
}

void ChannelTemplate::SetEventOpen(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    Unwrap(info)->set_channel_open_trigger(info.GetIsolate(), value->ToObject());
}

void ChannelTemplate::GetEventRecv(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    info.GetReturnValue().Set(Unwrap(info)->channel_recv_trigger(isolate));
}

void ChannelTemplate::SetEventRecv(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    Unwrap(info)->set_channel_recv_trigger(info.GetIsolate(), value->ToObject());
}

void ChannelTemplate::GetEventClosed(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    info.GetReturnValue().Set(Unwrap(info)->channel_closed_trigger(isolate));
}

void ChannelTemplate::SetEventClosed(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    Unwrap(info)->set_channel_closed_trigger(info.GetIsolate(), value->ToObject());
}

void ChannelTemplate::Send(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);

    if (args.Length() < 1) {
        // TODO(ghilbut): throw exception
        return;
    }

    v8::Local<v8::Value> arg0(args[0]);
    if (!arg0->IsString()) {
        // TODO(ghilbut): throw exception
        return;
    }

    v8::String::Utf8Value utf8(arg0);
    const char* param = *utf8;
    if (param) {
        Unwrap(args)->ChannelSend(param);
    } else {
        // TODO(ghilbut): throw exception
    }
}
