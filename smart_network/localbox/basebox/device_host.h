#ifndef DEVICE_HOST_H_
#define DEVICE_HOST_H_

#include "base/ref_implement.h"
#include "base/io_service_ref.h"
#include "basebox/device_context.h"
#include <v8.h>


class Storage;
class DeviceContext;
class TemplateFactory;

class DeviceHost : public RefImplement {
public:
    DeviceHost(const IOServiceRef& io_service, const char* basepath);
    ~DeviceHost(void);

    void FireServiceLoaded();
    void FireChannelOpend();

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
    v8::Local<v8::Object> channel_open_trigger(v8::Isolate* isolate) const;
    void set_channel_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);


private:
    DeviceContext context_;
    v8::Persistent<v8::Object> self_;
    v8::Persistent<v8::Object> on_service_load_;
    v8::Persistent<v8::Object> on_channel_open_;

    //mutable TemplateFactory template_factory_;
};

#endif  // DEVICE_HOST_H_