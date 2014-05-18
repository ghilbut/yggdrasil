#ifndef TEMPLATE_FACTORY_H_
#define TEMPLATE_FACTORY_H_

#include <v8.h>


class TemplateFactory {
public:
    TemplateFactory(v8::Isolate* isolate);
    ~TemplateFactory(void) {}

    // frontend
    v8::Local<v8::FunctionTemplate> HttpRequestTemplate(v8::Isolate* isolate) const;
    v8::Local<v8::Object> NewHttpRequest(v8::Isolate* isolate, void* opaque) const;
    v8::Local<v8::FunctionTemplate> HttpResponseTemplate(v8::Isolate* isolate) const;
    // NOTE(ghilbut): Response is only constructed from script.
    v8::Local<v8::FunctionTemplate> HttpWebSocketTemplate(v8::Isolate* isolate) const;
    v8::Local<v8::Object> NewHttpWebSocket(v8::Isolate* isolate, void* opaque) const;
    v8::Local<v8::FunctionTemplate> HttpObjectTemplate(v8::Isolate* isolate) const;
    v8::Local<v8::Object> NewHttpObject(v8::Isolate* isolate, void* opaque) const;

    // basebox
    v8::Local<v8::FunctionTemplate> DeviceTemplate(v8::Isolate* isolate) const;
    v8::Local<v8::Object> NewDevice(v8::Isolate* isolate, void* opaque) const;
    v8::Local<v8::FunctionTemplate> ServiceTemplate(v8::Isolate* isolate) const;
    v8::Local<v8::Object> NewService(v8::Isolate* isolate, void* opaque) const;
    v8::Local<v8::FunctionTemplate> ChannelTemplate(v8::Isolate* isolate) const;
    v8::Local<v8::Object> NewChannel(v8::Isolate* isolate, void* opaque) const;

private:
    v8::Persistent<v8::ObjectTemplate> http_req_header_;
    v8::Persistent<v8::FunctionTemplate> http_request_;
    v8::Persistent<v8::FunctionTemplate> http_response_;
    v8::Persistent<v8::FunctionTemplate> http_websocket_;
    v8::Persistent<v8::FunctionTemplate> http_object_;

    // basebox
    v8::Persistent<v8::FunctionTemplate> device_;
    v8::Persistent<v8::FunctionTemplate> service_;
    v8::Persistent<v8::FunctionTemplate> channel_;
};

#endif  // TEMPLATE_FACTORY_H_