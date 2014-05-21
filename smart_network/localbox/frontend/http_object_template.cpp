#include "http_object_template.h"

#include "http_message.h"
#include "http_response_template.h"
#include "basebox/service.h"


namespace Http {

template<typename T>
static Service* Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    //v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    //void* ptr = wrap->Value();
    //return static_cast<ServiceBroker*>(ptr);
    return static_cast<Service*>(object->GetAlignedPointerFromInternalField(0));
}

v8::Local<v8::FunctionTemplate> ObjectTemplate::New(v8::Isolate* isolate
                                                    , v8::Local<v8::FunctionTemplate> response) {

    v8::Local<v8::FunctionTemplate> ft = v8::FunctionTemplate::New(isolate);
    ft->SetClassName(v8::String::NewFromUtf8(isolate, "Http"));

    v8::Local<v8::ObjectTemplate> ot = ft->InstanceTemplate();
    ot->SetInternalFieldCount(1);
    ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onrequest")
        , &ObjectTemplate::GetEventRequest
        , &ObjectTemplate::SetEventRequest);
    ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onwebsocket")
        , &ObjectTemplate::GetEventWebSocket
        , &ObjectTemplate::SetEventWebSocket);
    ot->Set(isolate, "notify"
        , v8::FunctionTemplate::New(isolate, ObjectTemplate::Notify));

    ot->Set(isolate, "Response", response);

    return ft;
}

void ObjectTemplate::GetEventRequest(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    info.GetReturnValue().Set(Unwrap(info)->request_trigger(isolate));
}

void ObjectTemplate::SetEventRequest(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Unwrap(info)->set_request_trigger(isolate, value->ToObject());
}

void ObjectTemplate::GetEventWebSocket(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    info.GetReturnValue().Set(Unwrap(info)->open_trigger(isolate));
}

void ObjectTemplate::SetEventWebSocket(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Unwrap(info)->set_open_trigger(isolate, value->ToObject());
}

void ObjectTemplate::Notify(const v8::FunctionCallbackInfo<v8::Value>& args) {
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

    const int data_len = str->Utf8Length();
    char* data = new char[data_len];
    str->WriteUtf8(data, data_len);

    const Message msg(data, data_len);
    Unwrap(args)->HttpNotify(msg);
}

}  // namespace Http
