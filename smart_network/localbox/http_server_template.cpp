#include "http_server.h"
#include "http_server_template.h"
#include "http_response_template.h"
#include "http_message.h"
#include "environ.h"


namespace Http {

v8::Local<v8::FunctionTemplate> ServerTemplate::Get(Environ* env) {

    v8::Isolate* isolate = env->isolate();

    v8::Local<v8::FunctionTemplate> ft = v8::FunctionTemplate::New(isolate, Constructor);
    ft->Set(isolate, "listen", v8::FunctionTemplate::New(isolate, ServerTemplate::Listen));
    ft->Set(isolate, "close", v8::FunctionTemplate::New(isolate, ServerTemplate::Close));

    v8::Local<v8::ObjectTemplate> ot = ft->InstanceTemplate();
    ot->SetInternalFieldCount(1);

    ot->Set(v8::String::NewFromUtf8(isolate, "listen"), v8::FunctionTemplate::New(isolate, ServerTemplate::Listen));
    ot->Set(v8::String::NewFromUtf8(isolate, "close"), v8::FunctionTemplate::New(isolate, ServerTemplate::Close));
    ot->Set(v8::String::NewFromUtf8(isolate, "notify"), v8::FunctionTemplate::New(isolate, ServerTemplate::Notify));
    ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onrequest"), ServerTemplate::GetEventRequest, ServerTemplate::SetEventRequest);
    ot->SetAccessor(v8::String::NewFromUtf8(isolate, "onmessage"), ServerTemplate::GetEventMessage, ServerTemplate::SetEventMessage);

    TemplateFactory& tf = env->template_factory();
    ot->Set(v8::String::NewFromUtf8(isolate, "Response"), tf.ResponseTemplate(isolate));

    return ft;
}

void ServerTemplate::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    if (!args.IsConstructCall()) {
        const char* msg = "Failed to construct 'http.Server': Please use the 'new' operator, this DOM object constructor cannot be called as a function.";
        v8::Local<v8::Value> e = v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, msg));
        args.GetReturnValue().Set(isolate->ThrowException(e));
        return;
    }

    Environ* env = static_cast<Environ*>(isolate->GetData(0));
    Server* server = new Server(env);
   
    v8::Local<v8::Object> object = args.Holder();
    server->MakeWeak(isolate, object);
    args.GetReturnValue().Set(object);
}

v8::Local<v8::Object> ServerTemplate::NewInstance(Environ* env) {
    v8::Local<v8::FunctionTemplate> t = ServerTemplate::Get(env);
    v8::Local<v8::Function> c = t->GetFunction();
    v8::Local<v8::Object> obj = c->NewInstance();
    return obj;
}

template<typename T>
Server* ServerTemplate::Unwrap(T _t) {
    v8::Local<v8::Object> object = _t.Holder();
    //v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    //void* ptr = wrap->Value();
    //return static_cast<Server*>(ptr);
    return static_cast<Server*>(object->GetAlignedPointerFromInternalField(0));
}

void ServerTemplate::GetEventRequest(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Server* s = Unwrap(info);
    info.GetReturnValue().Set(s->request_trigger(info.GetIsolate()));
}

void ServerTemplate::SetEventRequest(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        Unwrap(info)->set_request_trigger(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }
}

void ServerTemplate::GetEventMessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Server* s = Unwrap(info);
    info.GetReturnValue().Set(s->message_trigger(info.GetIsolate()));
}

void ServerTemplate::SetEventMessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        Unwrap(info)->set_message_trigger(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }
}

void ServerTemplate::GetEventError(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    Server* s = Unwrap(info);
    info.GetReturnValue().Set(s->error_trigger(info.GetIsolate()));
}

void ServerTemplate::SetEventError(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        Unwrap(info)->set_error_trigger(info.GetIsolate(), func);
    } else {
        // TODO(ghilbut): throw js exception
    }
}

void ServerTemplate::Listen(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    Server* s = Unwrap(args);
    bool result = s->DoListen();
    args.GetReturnValue().Set(v8::Boolean::New(isolate, result));
}

void ServerTemplate::Close(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    Server* s = Unwrap(args);
    s->DoClose();
}

void ServerTemplate::Notify(const v8::FunctionCallbackInfo<v8::Value>& args) {

    if (args.Length() < 1) {
        // TODO(ghilbut): throw exception
        return;
    }

    if (!args[0]->IsString()) {
        // TODO(ghilbut): throw exception
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::String> str = args[0]->ToString();

    const int data_len = str->Utf8Length();
    char* data = new char[data_len];
    str->WriteUtf8(data, data_len);

    Message msg(data, data_len);

    Server* s = Unwrap(args);
    s->DoNotify(msg);
}

}  // namespace Http