#include "context.h"

#include "sample.h"
#include "js/file_object.h"


class Context::Wrapper{
public:
    Wrapper(void);
    ~Wrapper(void) {}

    v8::Isolate* isolate_;
    v8::Isolate::Scope isolate_scope_;
    v8::HandleScope handle_scope_;
};

Context::Wrapper::Wrapper(void)
    : isolate_(v8::Isolate::New())
    , isolate_scope_(isolate_)
    , handle_scope_(isolate_) {
    // nothing
}



Context::Context(const IOServiceRef& io_service, const char* basepath)
    : wrap_(new Wrapper())
    , isolate_(wrap_->isolate_)
    , io_service_(io_service) {

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

Context::~Context(void) {
    context_->Exit();
    delete wrap_;
    isolate_->Dispose();
}

v8::Isolate* Context::isolate(void) const {
    return isolate_;
}

v8::Handle<v8::Context> Context::context(void) const {
    return context_;
}
