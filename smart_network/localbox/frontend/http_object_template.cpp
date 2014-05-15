#include "http_object_template.h"

#include "service_broker.h"


namespace Http {

template<typename T>
static ServiceBroker* Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    //v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    //void* ptr = wrap->Value();
    //return static_cast<ServiceBroker*>(ptr);
    return static_cast<ServiceBroker*>(object->GetAlignedPointerFromInternalField(0));
}

v8::Local<v8::FunctionTemplate> ObjectTemplate::Get(v8::Isolate* isolate) {

    v8::Local<v8::FunctionTemplate> ft = v8::FunctionTemplate::New(isolate);
    ft->SetClassName(v8::String::NewFromUtf8(isolate, "Http"));
    ft->Set(isolate, "pause"
            , v8::FunctionTemplate::New(isolate, ObjectTemplate::Pause));
    ft->Set(isolate, "resume"
            , v8::FunctionTemplate::New(isolate, ObjectTemplate::Resume));

    v8::Local<v8::ObjectTemplate> ot = ft->InstanceTemplate();
    ot->SetInternalFieldCount(1);
    ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onrequest")
                    , &ObjectTemplate::GetEventRequest
                    , &ObjectTemplate::SetEventRequest);
    ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onwebsocket")
                    , &ObjectTemplate::GetEventWebSocket
                    , &ObjectTemplate::SetEventWebSocket);
    /*ot->Set(isolate, "pause"
            , v8::FunctionTemplate::New(isolate, ObjectTemplate::Pause));
    ot->Set(isolate, "resume"
            , v8::FunctionTemplate::New(isolate, ObjectTemplate::Resume));*/

    return ft;
}

void ObjectTemplate::GetEventRequest(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    ServiceBroker* s = Unwrap(info);
    info.GetReturnValue().Set(s->request_trigger(info.GetIsolate()));
}

void ObjectTemplate::SetEventRequest(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    //v8::Local<v8::Object> trigger = v8::Local<v8::Object>::Cast(value);
    //Unwrap(info)->set_request_trigger(info.GetIsolate(), trigger);
    //Unwrap(info)->set_request_trigger(info.GetIsolate(), value->ToObject());
    Unwrap(info)->set_request_trigger(info.GetIsolate(), v8::Object::New(info.GetIsolate()));
}

void ObjectTemplate::GetEventWebSocket(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    ServiceBroker* s = Unwrap(info);
    info.GetReturnValue().Set(s->open_trigger(info.GetIsolate()));
}

void ObjectTemplate::SetEventWebSocket(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    //v8::Local<v8::Object> trigger = v8::Local<v8::Object>::Cast(value);
    //Unwrap(info)->set_open_trigger(info.GetIsolate(), trigger);
    Unwrap(info)->set_open_trigger(info.GetIsolate(), value->ToObject());
}

void ObjectTemplate::Pause(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Unwrap(args)->HttpPause();
}

void ObjectTemplate::Resume(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Unwrap(args)->HttpResume();
}

void ObjectTemplate::Notify(const v8::FunctionCallbackInfo<v8::Value>& args) {

}

}  // namespace Http
