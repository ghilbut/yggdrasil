#include "http_websocket.h"
#include "http_websocket_template.h"
#include "http_message.h"


namespace Http {

v8::Local<v8::FunctionTemplate> WebSocketTemplate::Get(v8::Isolate* isolate) {

    v8::Local<v8::FunctionTemplate> ft = v8::FunctionTemplate::New(isolate);
    ft->SetClassName(v8::String::NewFromUtf8(isolate, "WebSocket"));

    v8::Handle<v8::ObjectTemplate> ot = ft->InstanceTemplate();
    ot->SetInternalFieldCount(1);
    //ot->SetAccessor(v8::String::NewFromUtf8(isolate, "statusCode"), GetStatusCode, SetStatusCode);
    //ot->Set(isolate, "getHeader", v8::FunctionTemplate::New(isolate, ResponseTemplate::GetHeader));
    //ot->Set(isolate, "setHeader", v8::FunctionTemplate::New(isolate, ResponseTemplate::SetHeader));
    //ot->Set(isolate, "removeHeader", v8::FunctionTemplate::New(isolate, ResponseTemplate::RemoveHeader));
    //ot->SetAccessor(v8::String::NewFromUtf8(isolate, "data"), GetData, SetData);
    ot->Set(isolate, "send", v8::FunctionTemplate::New(isolate, WebSocketTemplate::Send));

    return ft;
}

v8::Local<v8::Object> WebSocketTemplate::NewInstance(v8::Isolate* isolate, struct mg_connection* conn) {
    v8::Local<v8::FunctionTemplate> ft = WebSocketTemplate::Get(isolate);
    v8::Local<v8::Function> f = ft->GetFunction();
    v8::Local<v8::Object> i = f->NewInstance();

    Environ* env = static_cast<Environ*>(isolate->GetData(0));
    WebSocket* sock = new WebSocket(env, conn);
    sock->MakeWeak(isolate, i);
    i->SetAlignedPointerInInternalField(0, sock);
    //i->SetAlignedPointerInInternalField(0, conn);
    return i;
}

template<typename T>
WebSocket* WebSocketTemplate::Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    //v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    //void* ptr = wrap->Value();
    //return static_cast<Server*>(ptr);
    return static_cast<WebSocket*>(object->GetAlignedPointerFromInternalField(0));
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

    WebSocket* ws = Unwrap(args);
    ws->DoSend(msg);
}

}  // namespace Http
