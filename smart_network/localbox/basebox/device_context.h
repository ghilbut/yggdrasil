#ifndef DEVICE_CONTEXT_H_
#define DEVICE_CONTEXT_H_

#include "storage.h"
#include "template_factory.h"
#include "base/io_service.h"
#include "base/io_service_ref.h"
#include <v8.h>


class DeviceContext {
public:
    DeviceContext(const IOServiceRef& io_service
                  , const char* basepath);
    ~DeviceContext(void);

    v8::Isolate* isolate(void) const;
    v8::Handle<v8::Context> context(void) const;



    TemplateFactory& template_factory(void) const;
    Storage& storage(void) const;

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

    mutable TemplateFactory template_factory_;


    


    mutable Storage storage_;
};

#endif  // DEVICE_CONTEXT_H_