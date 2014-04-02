#include "localbox.h"
#include "sample.h"
#include "http.h"


LocalBox::LocalBox(v8::Isolate* isolate, const char* rootpath) 
    : work_(new boost::asio::io_service::work(io_service_))
    , thread_(boost::bind(&boost::asio::io_service::run, &io_service_))
    , isolate_(isolate)
    , handle_scope_(isolate_)
    , s_(rootpath) {

    isolate->SetData(0, &io_service_);

    v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(v8::String::NewFromUtf8(isolate, "print"), v8::FunctionTemplate::New(isolate, Print));
    global->Set(v8::String::NewFromUtf8(isolate, "read"), v8::FunctionTemplate::New(isolate, Read));
    global->Set(v8::String::NewFromUtf8(isolate, "load"), v8::FunctionTemplate::New(isolate, Load));
    global->Set(v8::String::NewFromUtf8(isolate, "quit"), v8::FunctionTemplate::New(isolate, Quit));
    global->Set(v8::String::NewFromUtf8(isolate, "version"), v8::FunctionTemplate::New(isolate, Version));

    global->Set(
        v8::String::NewFromUtf8(isolate, "Http")
        , Http::Template::New(isolate)
        , Http::kAttribute);


    v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global);

    
    
    context->Enter();



    BindSample(context);
    {
        v8::HandleScope handle_scope(context->GetIsolate());
        context->Global()->Set(
            v8::String::NewFromUtf8(isolate, "http")
            , Http::NewInstance(isolate)
            , Http::kAttribute);
    }

    context_ = context;



    service0_ = new ServiceBroker(isolate, s_);
    service1_ = new ServiceBroker(isolate, s_);
    service2_ = new ServiceBroker(isolate, s_);
}

LocalBox::~LocalBox(void) {
    context_->Exit();

    delete work_;
    io_service_.stop();
    thread_.join();

    delete service2_;
    delete service1_;
    delete service0_;
}

void LocalBox::RunShell(void) {
    v8::HandleScope handle_scope(isolate_);
    ::RunShell(context_);
}