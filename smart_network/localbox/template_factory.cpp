#include "template_factory.h"
#include "http_request_template.h"
#include "http_response_template.h"
#include "http_websocket_template.h"
#include "http_server_template.h"


TemplateFactory::TemplateFactory(v8::Isolate* isolate)
    : request_t_(isolate, Http::RequestTemplate::Get(isolate))
    , response_t_(isolate, Http::ResponseTemplate::Get(isolate))
    , websocket_t_(isolate, Http::WebSocketTemplate::Get(isolate)) {
    // nothing
}

v8::Local<v8::FunctionTemplate> TemplateFactory::RequestTemplate(v8::Isolate* isolate) const {
    return v8::Local<v8::FunctionTemplate>::New(isolate, request_t_);
}

v8::Local<v8::FunctionTemplate> TemplateFactory::ResponseTemplate(v8::Isolate* isolate) const {
    return v8::Local<v8::FunctionTemplate>::New(isolate, response_t_);
}

v8::Local<v8::FunctionTemplate> TemplateFactory::WebSocketTemplate(v8::Isolate* isolate) const {
    return v8::Local<v8::FunctionTemplate>::New(isolate, websocket_t_);
}