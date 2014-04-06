#include "localbox.h"
#include "sample.h"
#include "http.h"
#include "environ.h"


LocalBox::LocalBox(const char* rootpath) 
    : work_(new boost::asio::io_service::work(io_service_))
    , thread_(boost::bind(&boost::asio::io_service::run, &io_service_))
    , isolate_(v8::Isolate::GetCurrent())
    , handle_scope_(isolate_) {

    isolate_->SetData(0, &io_service_);

    v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate_);
    global->Set(v8::String::NewFromUtf8(isolate_, "print"), v8::FunctionTemplate::New(isolate_, Print));
    global->Set(v8::String::NewFromUtf8(isolate_, "read"), v8::FunctionTemplate::New(isolate_, Read));
    global->Set(v8::String::NewFromUtf8(isolate_, "load"), v8::FunctionTemplate::New(isolate_, Load));
    global->Set(v8::String::NewFromUtf8(isolate_, "quit"), v8::FunctionTemplate::New(isolate_, Quit));
    global->Set(v8::String::NewFromUtf8(isolate_, "version"), v8::FunctionTemplate::New(isolate_, Version));

    global->Set(
        v8::String::NewFromUtf8(isolate_, "Http")
        , Http::Template::New(isolate_)
        , Http::kAttribute);


    v8::Local<v8::Context> context = v8::Context::New(isolate_, NULL, global);

    
    
    context->Enter();



    BindSample(context);
    {
        v8::HandleScope handle_scope(context->GetIsolate());
        context->Global()->Set(
            v8::String::NewFromUtf8(isolate_, "http")
            , Http::NewInstance(isolate_)
            , Http::kAttribute);
    }

    context_ = context;

    service0_ = new ServiceBroker(io_service_, rootpath, 81);
    service1_ = new ServiceBroker(io_service_, rootpath, 82);
    service2_ = new ServiceBroker(io_service_, rootpath, 83);
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