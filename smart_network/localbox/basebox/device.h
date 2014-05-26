#ifndef DEVICE_H_
#define DEVICE_H_

#include "base/ref_object.h"
#include "base/io_service_ref.h"
#include "basebox/context.h"
#include <v8.h>
#include <boost/filesystem.hpp>


struct mg_connection;
class DeviceContext;
class TemplateFactory;

class Device : public RefObject {
public:
    Device(const IOServiceRef& io_service
        , const std::string& id
        , const std::string& root
        , const std::string& script_root
        , const std::string& static_url
        , const std::string& static_root
        , const std::string& template_root
        , const std::string& document_root);
    ~Device(void);

    void FireServiceLoad(v8::Local<v8::Object> service);
    void FireClosed(v8::Local<v8::Object> service);

    template <typename F>
    inline void Post(const F& handler) {
        context_.Post(handler);
    }

    bool IsStaticURI(const std::string& uri) const;
    int SendStaticFile(mg_connection* conn) const;

    v8::Isolate* isolate(void) const;
    v8::Handle<v8::Context> context(void) const;
    TemplateFactory& template_factory(void) const;

    const char* id(void) const;
    const char* name(void) const;
    const char* model(void) const;


public: // for javascript template
    v8::Local<v8::Object> self(v8::Isolate* isolate) const;
    v8::Local<v8::Object> service_load_trigger(v8::Isolate* isolate) const;
    void set_service_load_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);
    v8::Local<v8::Object> closed_trigger(v8::Isolate* isolate) const;
    void set_closed_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);


private:
    Context context_;
    mutable TemplateFactory template_factory_;

    v8::Persistent<v8::Object> self_;
    v8::Persistent<v8::Object> on_service_load_;
    v8::Persistent<v8::Object> on_closed_;




    
    const std::string id_;
    const std::string name_;
    const std::string model_;

    const std::string root_;
    const std::string script_root_;
    const std::string static_url_;
    const std::string static_root_;
    const std::string template_root_;
    const std::string document_root_;
};

#endif  // DEVICE_H_