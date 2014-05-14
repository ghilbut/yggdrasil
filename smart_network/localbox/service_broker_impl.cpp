#include "service_broker.h"
#include "service_broker_impl.h"
#include "storage.h"
#include "sample.h"
#include "http.h"
#include "template_factory.h"
#include "http_server_template.h"



ServiceBroker::ServiceBroker(boost::asio::io_service& io_service
                             , const char* basepath
                             , int port)
    : pimpl_(new ServiceBroker::Impl(io_service, basepath, port)) {
}

ServiceBroker::~ServiceBroker(void) {
}

void ServiceBroker::RunShell(void) {
    pimpl_->RunShell();
}

ServiceBroker::Impl::Impl(boost::asio::io_service& io_service
                          , const char* basepath
                          , int port)
    : env_(io_service, basepath, port)
    , storage_(env_.storage()) {

    v8::Isolate* isolate = env_.isolate();

    v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(v8::String::NewFromUtf8(isolate, "print"), v8::FunctionTemplate::New(isolate, Print));
    global->Set(v8::String::NewFromUtf8(isolate, "read"), v8::FunctionTemplate::New(isolate, Read));
    global->Set(v8::String::NewFromUtf8(isolate, "load"), v8::FunctionTemplate::New(isolate, Load));
    global->Set(v8::String::NewFromUtf8(isolate, "quit"), v8::FunctionTemplate::New(isolate, Quit));
    global->Set(v8::String::NewFromUtf8(isolate, "version"), v8::FunctionTemplate::New(isolate, Version));



    isolate->SetData(0, &env_);

    context_ = v8::Context::New(isolate, NULL, global);
    //context_ = CreateShellContext(isolate);
    context_->Enter();



    BindSample(context_);

    context_->Global()->Set(
        v8::String::NewFromUtf8(isolate, "http")
        , Http::ServerTemplate::NewInstance(&env_)
        , Http::kAttribute);

    



    v8::Handle<v8::String> source = ReadFile(isolate, storage_.settings());
    if (source.IsEmpty()) {
        //return -1;
    }
    if (!ExecuteString(isolate,
        source,
        v8::String::NewFromUtf8(isolate, storage_.settings()),
        true, //false,
        true)) { //false)) {
            //return -1;
    }
}

ServiceBroker::Impl::~Impl(void) {

    context_->Exit();

    //delete work_;
    //io_service_.stop();
    //thread_.join();
}

void ServiceBroker::Impl::RunShell(void) {
    v8::HandleScope handle_scope(env_.isolate());
    ::RunShell(context_);
}