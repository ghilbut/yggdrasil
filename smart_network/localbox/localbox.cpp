#include "localbox.h"
#include "sample.h"
#include "http.h"
#include "environ.h"
#include <mongoose.h>


static int http_request_handler(struct mg_connection* conn, enum mg_event ev) {
    LocalBox* s = static_cast<LocalBox*>(conn->server_param);
    return s->HttpRequest(conn, ev);
};

LocalBox::LocalBox(const char* rootpath) 
    : work_(new boost::asio::io_service::work(io_service_))
    , thread_(boost::bind(&boost::asio::io_service::run, &io_service_))
    , isolate_(v8::Isolate::GetCurrent())
    , handle_scope_(isolate_)
    , alive_(false)
    , stop_(false) {

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





    server0_ = new Http::Server();
    server0_->Create(this, http_request_handler, 81);
    //server1_ = new Http::Server();
    //server1_->Create(this, http_request_handler, 82);
    //server2_ = new Http::Server();
    //server2_->Create(this, http_request_handler, 83);

    service0_ = new ServiceBroker(io_service_, rootpath, 81);
    //service1_ = new ServiceBroker(io_service_, rootpath, 82);
    //service2_ = new ServiceBroker(io_service_, rootpath, 83);

    services_[81] = service0_;
    //services_[82] = service1_;
    //services_[83] = service2_;

    stop_ = false;
    //thread2_.swap(boost::thread(boost::bind(&LocalBox::thread2_main, this)));
}

LocalBox::~LocalBox(void) {

    stop_ = true;
    if (thread2_.joinable()) {
        thread2_.join();
    }

    context_->Exit();

    delete work_;
    io_service_.stop();
    thread_.join();

    //server2_->Destroy();
    //delete server2_;
    //server1_->Destroy();
    //delete server1_;
    server0_->Destroy();
    delete server0_;



    //delete service2_;
    //delete service1_;
    delete service0_;
}

int LocalBox::HttpRequest(struct mg_connection* conn, enum mg_event ev) {
    const int port = conn->local_port;
    ServiceMap::iterator itr = services_.find(port);
    if (itr == services_.end()) {
        return MG_FALSE;
    }

    ServiceBroker* s = itr->second;
    return s->HttpRequest(conn, ev);
};

void LocalBox::RunShell(void) {
    //v8::HandleScope handle_scope(isolate_);
    //::RunShell(context_);
    service0_->RunShell();
}






void LocalBox::thread2_main(void) {
    alive_ = true;
    while (!stop_) {
        server0_->Poll(10);
        server1_->Poll(10);
        server2_->Poll(10);
    }
    alive_ = false;
}