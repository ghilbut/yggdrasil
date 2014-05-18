#include "service_broker_impl.h"

#include "frontend/http_object_template.h"
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

void ServiceBroker::HttpNotify(const Http::Message& msg) {
    pimpl_->HttpNotify(msg);
}

v8::Local<v8::Object> ServiceBroker::request_trigger(v8::Isolate* isolate) const {
    return pimpl_->request_trigger(isolate);
}

void ServiceBroker::set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger) {
    pimpl_->set_request_trigger(isolate, trigger);
}

v8::Local<v8::Object> ServiceBroker::open_trigger(v8::Isolate* isolate) const {
    return pimpl_->open_trigger(isolate);
}

void ServiceBroker::set_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger) {
    pimpl_->set_open_trigger(isolate, trigger);
}

ServiceBroker::Impl::Impl(boost::asio::io_service& io_service
                          , const char* basepath
                          , int port)
    : env_(io_service, basepath, port)
    , storage_(env_.storage())
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





    //v8::Local<v8::Object> http = Http::ObjectTemplate::NewInstance(isolate, this);
    const v8::PropertyAttribute kAttribute = static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
    TemplateFactory& tf = env_.template_factory();

    v8::Local<v8::Object> http = tf.NewHttpObject(isolate, this);
    http_.Reset(isolate, http);
    context_->Global()->Set(
        v8::String::NewFromUtf8(isolate, "http")
        , http
        , kAttribute);

    v8::Local<v8::Object> device = tf.NewDevice(isolate, this);
    device_.Reset(isolate, device);
    context_->Global()->Set(
        v8::String::NewFromUtf8(isolate, "device")
        , device
        , kAttribute);





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

void ServiceBroker::Impl::HttpNotify(const Http::Message& msg) {
    ws_manager_.DoNotify(msg);
}

v8::Local<v8::Object> ServiceBroker::Impl::request_trigger(v8::Isolate* isolate) const {
    return req_manager_.request_trigger(isolate);
}

void ServiceBroker::Impl::set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger) {
    req_manager_.set_request_trigger(isolate, trigger);
}

v8::Local<v8::Object> ServiceBroker::Impl::open_trigger(v8::Isolate* isolate) const {
    return ws_manager_.open_trigger(isolate);
}

void ServiceBroker::Impl::set_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    ws_manager_.set_open_trigger(isolate, trigger);
}
