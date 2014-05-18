#include "channel_template.h"

#include "basebox/channel.h"
//#include "basebox/service_broker.h"
//#include "basebox/service_broker_impl.h"



template<typename T>
static Channel* Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    //v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    //void* ptr = wrap->Value();
    //return static_cast<ServiceBroker*>(ptr);
    return static_cast<Channel*>(object->GetAlignedPointerFromInternalField(0));
}

v8::Local<v8::FunctionTemplate> ChannelTemplate::New(v8::Isolate* isolate) {

        v8::Local<v8::FunctionTemplate> ft = v8::FunctionTemplate::New(isolate);
        ft->SetClassName(v8::String::NewFromUtf8(isolate, "Device"));

        v8::Local<v8::ObjectTemplate> ot = ft->InstanceTemplate();
        ot->SetInternalFieldCount(1);
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

void ChannelTemplate::GetEventRecv(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    //ServiceBroker::Impl* s = Unwrap(info);
    //info.GetReturnValue().Set(s->request_trigger(info.GetIsolate()));
}

void ChannelTemplate::SetEventRecv(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    //Unwrap(info)->set_request_trigger(info.GetIsolate(), value->ToObject());
}

void ChannelTemplate::GetEventClosed(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    //ServiceBroker::Impl* s = Unwrap(info);
    //info.GetReturnValue().Set(s->request_trigger(info.GetIsolate()));
}

void ChannelTemplate::SetEventClosed(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    //Unwrap(info)->set_request_trigger(info.GetIsolate(), value->ToObject());
}

void ChannelTemplate::Send(const v8::FunctionCallbackInfo<v8::Value>& args) {
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
