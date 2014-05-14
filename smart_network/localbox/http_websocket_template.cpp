#include "http_websocket.h"
#include "http_websocket_impl.h"
#include "http_websocket_template.h"
#include "http_message.h"


namespace Http {

v8::Local<v8::FunctionTemplate> WebSocketTemplate::Get(v8::Isolate* isolate) {

    v8::Local<v8::FunctionTemplate> ft = v8::FunctionTemplate::New(isolate);
    ft->SetClassName(v8::String::NewFromUtf8(isolate, "WebSocket"));

    v8::Handle<v8::ObjectTemplate> ot = ft->InstanceTemplate();
    ot->SetInternalFieldCount(1);
    ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onmessage"), GetEventMessage, SetEventMessage);
    ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onclosed"), GetEventClosed, SetEventClosed);
    ot->Set(isolate, "send", v8::FunctionTemplate::New(isolate, WebSocketTemplate::Send));

    return ft;
}

template<typename T>
static WebSocket::Impl* Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    //v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    //void* ptr = wrap->Value();
    //return static_cast<Server*>(ptr);
    return static_cast<WebSocket::Impl*>(object->GetAlignedPointerFromInternalField(0));
}

void WebSocketTemplate::GetEventMessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    WebSocket::Impl* ws = Unwrap(info);
    info.GetReturnValue().Set(ws->message_trigger(info.GetIsolate()));
}

void WebSocketTemplate::SetEventMessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::Local<v8::Object> trigger = v8::Local<v8::Object>::Cast(value);
    Unwrap(info)->set_message_trigger(info.GetIsolate(), trigger);
}

void WebSocketTemplate::GetEventClosed(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    WebSocket::Impl* ws = Unwrap(info);
    info.GetReturnValue().Set(ws->closed_trigger(info.GetIsolate()));
}

void WebSocketTemplate::SetEventClosed(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::Local<v8::Object> trigger = v8::Local<v8::Object>::Cast(value);
    Unwrap(info)->set_closed_trigger(info.GetIsolate(), trigger);
}

void WebSocketTemplate::Send(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    if (args.Length() < 1) {
        // TODO(ghilbut): throw exception
        return;
    }

    v8::Local<v8::Value> arg0 = args[0];
    if (!arg0->IsString()) {
        // TODO(ghilbut): throw exception
        return;
    }

    v8::Local<v8::String> str = arg0->ToString();

    const int data_len = str->Utf8Length();
    char* data = new char[data_len];
    str->WriteUtf8(data, data_len);

    Message msg(data, data_len);

    WebSocket::Impl* ws = Unwrap(args);
    ws->DoSend(msg);
}

}  // namespace Http
