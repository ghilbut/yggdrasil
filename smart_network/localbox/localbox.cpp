#include "localbox.h"
#include "sample.h"
#include "basebox/device_ref.h"
#include "basebox/service.h"


static int http_request_handler(struct mg_connection* conn, enum mg_event ev) {
    LocalBox* s = static_cast<LocalBox*>(conn->server_param);
    return s->HttpRequest(conn, ev);
};

LocalBox::LocalBox(const char* rootpath) 
    : isolate_(v8::Isolate::GetCurrent())
    , handle_scope_(isolate_) {

    isolate_->SetData(0, &io_service_);

    v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate_);
    global->Set(v8::String::NewFromUtf8(isolate_, "print"), v8::FunctionTemplate::New(isolate_, Print));
    global->Set(v8::String::NewFromUtf8(isolate_, "read"), v8::FunctionTemplate::New(isolate_, Read));
    global->Set(v8::String::NewFromUtf8(isolate_, "load"), v8::FunctionTemplate::New(isolate_, Load));
    global->Set(v8::String::NewFromUtf8(isolate_, "quit"), v8::FunctionTemplate::New(isolate_, Quit));
    global->Set(v8::String::NewFromUtf8(isolate_, "version"), v8::FunctionTemplate::New(isolate_, Version));

    /*global->Set(
        v8::String::NewFromUtf8(isolate_, "Http")
        , Http::Template::New(isolate_)
        , Http::kAttribute);*/


    v8::Local<v8::Context> context = v8::Context::New(isolate_, NULL, global);

    
    
    context->Enter();



    BindSample(context);
    /*{
        v8::HandleScope handle_scope(context->GetIsolate());
        context->Global()->Set(
            v8::String::NewFromUtf8(isolate_, "http")
            , Http::NewInstance(isolate_)
            , Http::kAttribute);
    }*/

    context_ = context;




    
    //IOServiceRef io_service;
    DeviceRef device(io_service_, rootpath);

    service0_ = ServiceRef(device);
    service1_ = ServiceRef(device);
    service2_ = ServiceRef(device);

    servers_.Create(81, boost::bind(&Service::HttpRequest, service0_.Get(), _1, _2));
    servers_.Create(82, boost::bind(&Service::HttpRequest, service1_.Get(), _1, _2));
    servers_.Create(83, boost::bind(&Service::HttpRequest, service2_.Get(), _1, _2));
}

LocalBox::~LocalBox(void) {

    context_->Exit();

    servers_.Destroy(83);
    servers_.Destroy(82);
    servers_.Destroy(81);
}

void LocalBox::RunShell(void) {
    //v8::HandleScope handle_scope(isolate_);
    //::RunShell(context_);
    service0_->RunShell();
    //service1_->RunShell();
}
