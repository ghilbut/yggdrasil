#include "service_broker.h"
#include "service_broker_impl.h"

#include "frontend/http_request.h"
#include "storage.h"
#include "sample.h"
#include "http.h"
#include "template_factory.h"
#include "http_server_template.h"
#include <mongoose.h>


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

int ServiceBroker::HttpRequest(struct mg_connection* conn, enum mg_event ev) {
    return pimpl_->HttpRequest(conn, ev);
}

void ServiceBroker::HttpPause(void) {
    pimpl_->HttpPause();
}

void ServiceBroker::HttpResume(void) {
    pimpl_->HttpResume();
}

ServiceBroker::Impl::Impl(boost::asio::io_service& io_service
                          , const char* basepath
                          , int port)
    : env_(io_service, basepath, port)
    , storage_(env_.storage())
    , http_paused_(false)
    , req_manager_(&env_, http_)
    , ws_manager_(&env_, http_) {

    v8::Isolate* isolate = env_.isolate();
    //v8::Isolate::Scope isolate_scope(isolate);
    //v8::HandleScope handle_scope(isolate);

    


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





    /*
    v8::Local<v8::FunctionTemplate> ft = (env_.template_factory()).WebSocketTemplate(isolate);
    v8::Local<v8::Function> f = ft->GetFunction();
    v8::Local<v8::Object> http = f->NewInstance();
    http->SetAlignedPointerInInternalField(0, this);
    //this->AddRef();

    http_.Reset(isolate, http);
    //self_.MarkIndependent();
    //self_.SetWeak(this, &WebSocket::Impl::WeakCallback);
    */

    



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



int ServiceBroker::Impl::HttpRequest(struct mg_connection* conn, enum mg_event ev) {

    if (http_paused_) {
        return MG_FALSE;
    }

    if (ev == MG_REQUEST) {
        if (conn->is_websocket == 0) {
            Http::Response res = req_manager_.HandleRequest(conn);
            res.Send(conn);
        } else {
            ws_manager_.HandleMessage(conn);
        }
        return MG_TRUE;
    }

    if (ev == MG_AUTH) {
        return MG_TRUE;
    }

    return MG_FALSE;
}

void ServiceBroker::Impl::HttpPause(void) {
    http_paused_ = false;
}

void ServiceBroker::Impl::HttpResume(void) {
    http_paused_ = true;
}