#ifndef TEMPLATE_FACTORY_H_
#define TEMPLATE_FACTORY_H_

#include <v8.h>

class TemplateFactory {
public:
    TemplateFactory(v8::Isolate* isolate);
    ~TemplateFactory(void);

    v8::Local<v8::FunctionTemplate> ServerTemplate(v8::Isolate* isolate) const;
    v8::Local<v8::FunctionTemplate> RequestTemplate(v8::Isolate* isolate) const;
    v8::Local<v8::FunctionTemplate> ResponseTemplate(v8::Isolate* isolate) const;

private:
    v8::Persistent<v8::FunctionTemplate> request_t_;
    v8::Persistent<v8::FunctionTemplate> response_t_;
    v8::Persistent<v8::FunctionTemplate> server_t_;
};

#endif  // TEMPLATE_FACTORY_H_