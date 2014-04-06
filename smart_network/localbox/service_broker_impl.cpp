#include "service_broker.h"
#include "service_broker_impl.h"
#include "storage.h"
#include "sample.h"
#include "http.h"
#include "template_factory.h"



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
    , storage_(env_.storage())
    , isolate_(env_.isolate())
    , isolate_scope_(isolate_)
    , handle_scope_(isolate_) {


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

    TemplateFactory* tf = new TemplateFactory(isolate_);
    isolate_->SetData(0, &env_);
    isolate_->SetData(1, tf);
    //TemplateFactory* tf = static_cast<TemplateFactory*>(isolate_->GetData(1));
    global->Set(v8::String::NewFromUtf8(isolate_, "Request"), tf->RequestTemplate(isolate_));
    global->Set(v8::String::NewFromUtf8(isolate_, "Response"), tf->ResponseTemplate(isolate_));
    global->Set(v8::String::NewFromUtf8(isolate_, "Server"), tf->ServerTemplate(isolate_));


    context_ = v8::Context::New(isolate_, NULL, global);
    //context_ = CreateShellContext(isolate_);
    context_->Enter();



    BindSample(context_);
    {
        v8::HandleScope handle_scope(context_->GetIsolate());
        context_->Global()->Set(
            v8::String::NewFromUtf8(isolate_, "http")
            , Http::NewInstance(isolate_)
            , Http::kAttribute);
    }



    v8::Handle<v8::String> source = ReadFile(isolate_, storage_.settings());
    if (source.IsEmpty()) {
        //return -1;
    }
    if (!ExecuteString(isolate_,
        source,
        v8::String::NewFromUtf8(isolate_, storage_.settings()),
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
    ::RunShell(context_);
}