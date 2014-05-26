#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "template_factory.h"
#include "base/io_service.h"
#include "base/io_service_ref.h"
#include <v8.h>


class Context {
public:
    Context(const IOServiceRef& io_service
                  , const char* basepath);
    ~Context(void);

    v8::Isolate* isolate(void) const;
    v8::Handle<v8::Context> context(void) const;

    template <typename F>
    inline void Post(const F& handler) {
        io_service_->Post(handler);
    }


private:
    class Wrapper;
    Wrapper* wrap_;
    v8::Isolate* isolate_;
    v8::Handle<v8::Context> context_;
    IOServiceRef io_service_;
};

#endif  // CONTEXT_H_