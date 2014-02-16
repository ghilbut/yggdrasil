#include "http_object.h"
#include <mongoose.h>
//#include <v8.h>




/*
class Service {
public:
    Service(void) { }
    ~Service(void) {
        //onload_.Dispose();
    }

    static Service* Unwrap(v8::Handle<v8::Object> object);
    static void GetOnLoad(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetOnLoad(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

    static void Load(const v8::FunctionCallbackInfo<v8::Value>& args);

    static v8::Handle<v8::ObjectTemplate> MakeObjectTemplate(v8::Handle<v8::Context> context);

    v8::Persistent<v8::Function> onload_;
};

Service* Service::Unwrap(v8::Handle<v8::Object> object) {
    v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(object->GetInternalField(0));
    void* ptr = wrap->Value();
    return static_cast<Service*>(ptr);
}

void Service::GetOnLoad(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Object> self = info.Holder();
    Service* p = Unwrap(self);
    info.GetReturnValue().Set(p->onload_);
}

void Service::SetOnLoad(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::Local<v8::Object> self = info.Holder();
    Service* p = Unwrap(self);

    if (value->IsFunction()) {
        v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
        (p->onload_).Reset(info.GetIsolate(), func);
    }

    info.GetReturnValue().Set(value);
}

void Service::Load(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope context_scope(context);

    v8::Local<v8::Object> self = args.Holder();
    Service* p = Unwrap(self);

    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate, p->onload_);

    if (func.IsEmpty()) {
        return;
    }

    v8::Handle<v8::Value> params[2];
    params[0] = v8::Integer::New(isolate, 1);
    params[1] = v8::Integer::New(isolate, 2);

    v8::Local<v8::Object> global = context->Global();
    v8::Handle<v8::Value> js_result = func->Call(global, 2, params);

    if (js_result->IsInt32()) {
        int32_t result = (js_result->ToInt32())->Value();
        // do something with the result
        args.GetReturnValue().Set(v8::Integer::New(isolate, result));
    }
}
*/




static int request_handle(struct mg_connection* conn) {
    mg_printf_data(conn, "Hello! Requested URI is [%s]", conn->uri);


    Http::Object* object = static_cast<Http::Object*>(conn->server_param);

    if (conn->is_websocket) {

    } else {
        //object->p->Load();
        object->FireRequest();

    }

    return MG_REQUEST_PROCESSED;
}


namespace Http {

Object::Object(void) 
    : alive_(false), stop_(false) {
    // nothing
}

Object::~Object(void) {
    if (alive_) {
        Stop();
    }
}

bool Object::Start(void) {
    if (alive_) {
        return false;
    }

    stop_ = false;
    thread_.swap(boost::thread(boost::bind(&Object::thread_main, this)));
    return true;
}

void Object::Stop(void) {
    stop_ = true;
    thread_.join();
}

void Object::thread_main(void) {

    alive_ = true;

    mg_server* server = mg_create_server(this);
    mg_set_option(server, "listening_port", "80");
    mg_set_request_handler(server, request_handle);
    /*
    v8::V8::InitializeICU();
    v8::Isolate* isolate = v8::Isolate::New();
    v8::HandleScope handle_scope(isolate);

    v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    v8::Handle<v8::Context> context = v8::Context::New(isolate, NULL, global);
    context->Enter();

    {
        //Service* p = new Service();
        p = new Service();

        v8::Handle<v8::ObjectTemplate> object_t = v8::ObjectTemplate::New(isolate);
        object_t->SetInternalFieldCount(1);
        const char* name_onload = "onload";
        const char* name_load = "load";
        object_t->SetAccessor(v8::String::NewFromUtf8(isolate, name_onload), Service::GetOnLoad, Service::SetOnLoad);
        object_t->Set(v8::String::NewFromUtf8(isolate, name_load), v8::FunctionTemplate::New(isolate, Service::Load));

        v8::Handle<v8::Object> object = object_t->NewInstance();
        object->SetInternalField(0, v8::External::New(isolate, p));
        const char* name_Service = "Service";
        context->Global()->Set(v8::String::NewFromOneByte(isolate, (const uint8_t*)name_Service), object);
    }
    */
    while (!stop_) {
        mg_poll_server(server, 1000);
    }
    /*
    context->Exit();
    isolate->Dispose();
    */
    mg_destroy_server(&server);

    alive_ = false;
}

}