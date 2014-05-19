#include "template_factory.h"
#include "frontend/http_object_template.h"
#include "frontend/http_request_template.h"
#include "frontend/http_response_template.h"
#include "frontend/http_websocket_template.h"
//#include "http_server_template.h"
#include "frontend/http_request.h"

#include "base_object/device_template.h"
#include "base_object/service_template.h"
#include "base_object/channel_template.h"

#include "base_object/file_object.h"


static v8::Local<v8::Object> NewObject(v8::Isolate* isolate, void* opaque
                                       , v8::Local<v8::FunctionTemplate>& ft) {
    v8::Local<v8::Function> f = ft->GetFunction();
    v8::Local<v8::Object> obj = f->NewInstance();
    obj->SetAlignedPointerInInternalField(0, opaque);
    return obj;
}

TemplateFactory::TemplateFactory(v8::Isolate* isolate) {

    // frontend
    v8::Local<v8::ObjectTemplate>   http_req_header = Http::RequestTemplate::NewHeader(isolate);
    http_req_header_.Reset(isolate, http_req_header);
    v8::Local<v8::FunctionTemplate> http_request    = Http::RequestTemplate::New(isolate);
    http_request_.Reset(isolate, http_request);
    v8::Local<v8::FunctionTemplate> http_response   = Http::ResponseTemplate::New(isolate);
    http_response_.Reset(isolate, http_response);
    v8::Local<v8::FunctionTemplate> http_websocket  = Http::WebSocketTemplate::New(isolate);
    http_websocket_.Reset(isolate, http_websocket);
    v8::Local<v8::FunctionTemplate> http_object     = Http::ObjectTemplate::New(isolate, http_response);
    http_object_.Reset(isolate, http_object);

    // basebox
    v8::Local<v8::FunctionTemplate> device = DeviceTemplate::New(isolate);
    device_.Reset(isolate, device);
    v8::Local<v8::FunctionTemplate> service = ServiceTemplate::New(isolate);
    service_.Reset(isolate, device);
    v8::Local<v8::FunctionTemplate> channel = ChannelTemplate::New(isolate);
    channel_.Reset(isolate, channel);

    //
    v8::Local<v8::FunctionTemplate> file_ = FileTemplate::New(isolate);
}

v8::Local<v8::FunctionTemplate> TemplateFactory::HttpRequestTemplate(v8::Isolate* isolate) const {
    return v8::Local<v8::FunctionTemplate>::New(isolate, http_request_);
}

v8::Local<v8::Object> TemplateFactory::NewHttpRequest(v8::Isolate* isolate, void* opaque) const {

    v8::Local<v8::ObjectTemplate> ot = v8::Local<v8::ObjectTemplate>::New(isolate, http_req_header_);
    v8::Local<v8::Object> headers = ot->NewInstance();
    headers->SetAlignedPointerInInternalField(0, opaque);

    v8::Local<v8::Object> obj = NewObject(isolate, opaque, HttpRequestTemplate(isolate));
    obj->Set(v8::String::NewFromUtf8(isolate, "headers"), headers);

    Http::Request* request = static_cast<Http::Request*>(opaque);
    request->MakeWeak(isolate, obj);
    return obj;
}

v8::Local<v8::FunctionTemplate> TemplateFactory::HttpResponseTemplate(v8::Isolate* isolate) const {
    return v8::Local<v8::FunctionTemplate>::New(isolate, http_response_);
}

// NOTE(ghilbut): 'NewHttpResponse' method is not exists.

v8::Local<v8::FunctionTemplate> TemplateFactory::HttpWebSocketTemplate(v8::Isolate* isolate) const {
    return v8::Local<v8::FunctionTemplate>::New(isolate, http_websocket_);
}

v8::Local<v8::Object> TemplateFactory::NewHttpWebSocket(v8::Isolate* isolate, void* opaque) const {
    return NewObject(isolate, opaque, HttpWebSocketTemplate(isolate));
}

v8::Local<v8::FunctionTemplate> TemplateFactory::HttpObjectTemplate(v8::Isolate* isolate) const {
    return v8::Local<v8::FunctionTemplate>::New(isolate, http_object_);
}

v8::Local<v8::Object> TemplateFactory::NewHttpObject(v8::Isolate* isolate, void* opaque) const {
    return NewObject(isolate, opaque, HttpObjectTemplate(isolate));
}

v8::Local<v8::FunctionTemplate> TemplateFactory::DeviceTemplate(v8::Isolate* isolate) const {
    return v8::Local<v8::FunctionTemplate>::New(isolate, device_);
}

v8::Local<v8::Object> TemplateFactory::NewDevice(v8::Isolate* isolate, void* opaque) const {
    return NewObject(isolate, opaque, DeviceTemplate(isolate));
}

v8::Local<v8::FunctionTemplate> TemplateFactory::ServiceTemplate(v8::Isolate* isolate) const {
    return v8::Local<v8::FunctionTemplate>::New(isolate, service_);
}

v8::Local<v8::Object> TemplateFactory::NewService(v8::Isolate* isolate, void* opaque) const {
    return NewObject(isolate, opaque, ServiceTemplate(isolate));
}

v8::Local<v8::FunctionTemplate> TemplateFactory::ChannelTemplate(v8::Isolate* isolate) const {
    return v8::Local<v8::FunctionTemplate>::New(isolate, channel_);
}

v8::Local<v8::Object> TemplateFactory::NewChannel(v8::Isolate* isolate, void* opaque) const {
    return NewObject(isolate, opaque, ChannelTemplate(isolate));
}
