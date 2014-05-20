#include "device_context.h"

#include "base_object/file_object.h"
#include "sample.h"


class DeviceContext::Wrapper{
public:
    Wrapper(void);
    ~Wrapper(void) {}

    v8::Isolate* isolate_;
    v8::Isolate::Scope isolate_scope_;
    v8::HandleScope handle_scope_;
};

DeviceContext::Wrapper::Wrapper(void)
    : isolate_(v8::Isolate::New())
    , isolate_scope_(isolate_)
    , handle_scope_(isolate_) {
    // nothing
}



DeviceContext::DeviceContext(const IOServiceRef& io_service
                             , const char* basepath)
    : wrap_(new Wrapper())
    , isolate_(wrap_->isolate_)    
    , template_factory_(isolate_)
    , io_service_(io_service)
    , storage_(basepath) {
    



    v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate_);
    global->Set(v8::String::NewFromUtf8(isolate_, "print"), v8::FunctionTemplate::New(isolate_, Print));
    global->Set(v8::String::NewFromUtf8(isolate_, "read"), v8::FunctionTemplate::New(isolate_, Read));
    global->Set(v8::String::NewFromUtf8(isolate_, "load"), v8::FunctionTemplate::New(isolate_, Load));
    global->Set(v8::String::NewFromUtf8(isolate_, "quit"), v8::FunctionTemplate::New(isolate_, Quit));
    global->Set(v8::String::NewFromUtf8(isolate_, "version"), v8::FunctionTemplate::New(isolate_, Version));

    global->Set(isolate_, "File", FileTemplate::New(isolate_));

    //isolate->SetData(0, &env_);

    context_ = v8::Context::New(isolate_, NULL, global);

    context_->Enter();
}

DeviceContext::~DeviceContext(void) {
    context_->Exit();
    delete wrap_;
    isolate_->Dispose();
}

v8::Isolate* DeviceContext::isolate(void) const {
    return isolate_;
}

v8::Handle<v8::Context> DeviceContext::context(void) const {
    return context_;
}

TemplateFactory& DeviceContext::template_factory(void) const {
    return template_factory_;
}

Storage& DeviceContext::storage(void) const {
    return storage_;
}
