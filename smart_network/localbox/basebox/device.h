#ifndef DEVICE_H_
#define DEVICE_H_

#include "base/ref_object.h"
#include "base/io_service_ref.h"
#include "basebox/context.h"
#include <v8.h>


class Storage;
class DeviceContext;
class TemplateFactory;

class Device : public RefObject {
public:
    Device(const IOServiceRef& io_service, const char* basepath);
    ~Device(void);

    void FireServiceLoad(v8::Local<v8::Object> service);
    void FireClosed(v8::Local<v8::Object> service);

    template <typename F>
    inline void Post(const F& handler) {
        context_.Post(handler);
    }

    v8::Isolate* isolate(void) const;
    v8::Handle<v8::Context> context(void) const;
    TemplateFactory& template_factory(void) const;
    Storage& storage(void) const;


public: // for javascript template
    v8::Local<v8::Object> self(v8::Isolate* isolate) const;
    v8::Local<v8::Object> service_load_trigger(v8::Isolate* isolate) const;
    void set_service_load_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);
    v8::Local<v8::Object> closed_trigger(v8::Isolate* isolate) const;
    void set_closed_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);


private:
    Context context_;
    mutable TemplateFactory template_factory_;
    mutable Storage storage_;

    v8::Persistent<v8::Object> self_;
    v8::Persistent<v8::Object> on_service_load_;
    v8::Persistent<v8::Object> on_closed_;
};

#endif  // DEVICE_H_