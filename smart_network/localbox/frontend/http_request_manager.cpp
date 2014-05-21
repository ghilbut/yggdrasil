#include "http_request_manager.h"

#include "http_request.h"
#include "http_request_template.h"
#include "http_response_template.h"
#include "http_response_impl.h"
#include "basebox/device.h"
#include "basebox/template_factory.h"
#include <mongoose.h>


namespace Http {

RequestManager::RequestManager(const DeviceRef& device, v8::Persistent<v8::Object>& caller) 
    : device_(device)
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
    device_->Post(boost::bind(&RequestManager::event_request, this, conn, promise_setter));

    return promise.get_future().get();
}

v8::Local<v8::Object> RequestManager::request_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_request_);
}

void RequestManager::set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger) {
    on_request_.Reset(isolate, trigger);
}

// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

void RequestManager::event_request(struct mg_connection *conn, boost::function<void(const Response&)> ret_setter) {
    v8::Isolate* isolate = device_->isolate();
    v8::Isolate::Scope isolatescope(isolate);
    v8::HandleScope handle_scope(isolate);

    TemplateFactory& tf = device_->template_factory();

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





    v8::TryCatch try_catch;
    v8::Local<v8::Value> retval = obj->CallAsFunction(caller, 1, params);
    if (try_catch.HasCaught()) {

        v8::String::Utf8Value exception(try_catch.Exception());
        const char* exception_string = ToCString(exception);
        v8::Handle<v8::Message> message = try_catch.Message();
        if (message.IsEmpty()) {
            // TODO(ghilbut): ?
        } else {
            // Print (filename):(line number): (message).
            v8::String::Utf8Value filename(message->GetScriptResourceName());
            const char* filename_string = ToCString(filename);
            int linenum = message->GetLineNumber();
            fprintf(stderr, "%s:%i: %s\n", filename_string, linenum, exception_string);
            // Print line of source code.
            v8::String::Utf8Value sourceline(message->GetSourceLine());
            const char* sourceline_string = ToCString(sourceline);
            fprintf(stderr, "%s\n", sourceline_string);
            // Print wavy underline (GetUnderline is deprecated).
            int start = message->GetStartColumn();
            for (int i = 0; i < start; i++) {
                fprintf(stderr, " ");
            }
            int end = message->GetEndColumn();
            for (int i = start; i < end; i++) {
                fprintf(stderr, "^");
            }
            fprintf(stderr, "\n");
            v8::String::Utf8Value stack_trace(try_catch.StackTrace());
            if (stack_trace.length() > 0) {
                const char* stack_trace_string = ToCString(stack_trace);
                fprintf(stderr, "%s\n", stack_trace_string);
            }
        }





        static const char* text = "server error";
        Response res;
        res.Reset(Response::Impl::New());
        res.set_status_code(500);
        res.set_data(exception_string, strlen(exception_string));
        ret_setter(res);
        return;
    }

    if (retval.IsEmpty()) {
        static const char* text = "server error";
        Response res;
        res.Reset(Response::Impl::New());
        //res.Reset(static_cast<Response::Impl*>(obj->GetAlignedPointerFromInternalField(0)));
        res.set_status_code(500);
        res.set_data(text, strlen(text));
        ret_setter(res);
        return;
    }

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
