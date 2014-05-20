#include "http_request_manager.h"

#include "http_request.h"
#include "http_request_template.h"
#include "http_response_template.h"
#include "http_response_impl.h"
#include "basebox/environ.h"
#include <mongoose.h>


namespace Http {

RequestManager::RequestManager(DeviceContext* context, v8::Persistent<v8::Object>& caller) 
    : context_(context)
    , caller_(caller) {
    // nothing
}

RequestManager::~RequestManager(void) {
    // nothing
}

Response RequestManager::HandleRequest(struct mg_connection *conn) {
    boost::promise<Response> promise;

    void (boost::promise<Response>::*setter)(const Response&) = &boost::promise<Response>::set_value;
    boost::function<void(const Response&)> promise_setter = boost::bind(setter, &promise, _1);
    context_->Post(boost::bind(&RequestManager::event_request, this, conn, promise_setter));

    return promise.get_future().get();
}

v8::Local<v8::Object> RequestManager::request_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_request_);
}

void RequestManager::set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger) {
    on_request_.Reset(isolate, trigger);
}

void RequestManager::event_request(struct mg_connection *conn, boost::function<void(const Response&)> ret_setter) {
    v8::Isolate* isolate = context_->isolate();
    v8::Isolate::Scope isolatescope(isolate);
    v8::HandleScope handle_scope(isolate);

    TemplateFactory& tf = context_->template_factory();

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::New(isolate, on_request_);
    if (obj.IsEmpty() || !obj->IsFunction() || !obj->IsCallable()) {
        // TODO(ghilbut): error handling
        const std::string text("invalid request handler.");
        mg_send_status(conn, 500);
        mg_send_data(conn, text.c_str(), text.length());
        return;
    }

    v8::Handle<v8::Value> params[1];
    Request* req = new Request(conn);
    //params[0] = RequestTemplate::NewInstance(isolate, req);
    //params[0] = RequestTemplate::NewInstance(isolate, req);
    params[0] = tf.NewHttpRequest(isolate, req);
    v8::Local<v8::Object> caller = v8::Local<v8::Object>::New(isolate, caller_);
    v8::Local<v8::Value> retval = obj->CallAsFunction(caller, 1, params);

    if (retval->IsObject()) {
        v8::Local<v8::FunctionTemplate> rt = tf.HttpResponseTemplate(isolate);
        v8::Local<v8::Object> obj = retval->ToObject();
        if (rt->GetFunction() == obj->GetConstructor()) {
            Response res;
            res.Reset(static_cast<Response::Impl*>(obj->GetAlignedPointerFromInternalField(0)));
            ret_setter(res);
            return;
        }
    }

    v8::Local<v8::String> str = retval->ToString();

    const int len = str->Utf8Length();
    char* buf = new char[len];
    str->WriteUtf8(buf, len);

    Response res;
    res.Reset(Response::Impl::New());
    res.set_data(buf, len);
    ret_setter(res);

    delete[] buf;
}

}  // namespace Http
