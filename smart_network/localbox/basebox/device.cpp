#include "device.h"
#include "device_ref.h"

#include "sample.h"
#include "context.h"
#include "base_object/device_template.h"
#include <mongoose.h>
#include <json/json.h>
#include <fstream>


Device::Device(const IOServiceRef& io_service
        , const std::string& id
        , const std::string& root
        , const std::string& script_root
        , const std::string& static_url
        , const std::string& static_root
        , const std::string& template_root
        , const std::string& document_root)
    : RefObject()
    , context_(io_service, root_.c_str())
    , template_factory_(context_.isolate())
    , id_(id)
    , root_(root)
    , script_root_(script_root)
    , static_url_(static_url)
    , static_root_(static_root)
    , template_root_(template_root)
    , document_root_(document_root) {

    v8::Isolate* isolate = context_.isolate();
    v8::Local<v8::Context> context = context_.context();

    v8::Local<v8::Object> device = template_factory_.NewDevice(isolate, this);
    self_.Reset(isolate, device);
    this->AddRef();

    context->Global()->Set(v8::String::NewFromUtf8(isolate, "device"), device);





    boost::filesystem::path main(script_root);
    main /= "main.js";
    v8::Handle<v8::String> source = ReadFile(isolate, main.string().c_str());
    if (source.IsEmpty()) {
        //return -1;
    }
    if (!ExecuteString(isolate,
        source,
        v8::String::NewFromUtf8(isolate, "main.js"),
        true, //false,
        true)) { //false)) {
            //return -1;
    }
}

Device::~Device(void) {
    // nothing
}

void Device::FireServiceLoad(v8::Local<v8::Object> service) {
    v8::Isolate* isolate = context_.isolate(); 
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::New(isolate, on_service_load_);
    if (!obj.IsEmpty() && obj->IsFunction() && obj->IsCallable()) {
        v8::Local<v8::Object> caller = v8::Local<v8::Object>::New(isolate, self_);
        v8::Handle<v8::Value> params[1];
        params[0] = service;
        obj->CallAsFunction(caller, 1, params);
    }
}

bool Device::IsStaticURI(const std::string& uri) const {
    return (uri.find(static_url_) == 0);
}

int Device::SendStaticFile(mg_connection* conn) const {

    boost::filesystem::path filepath(static_root_);
    filepath /= conn->uri + static_url_.length();
    if (boost::filesystem::exists(filepath)) {
        mg_send_status(conn, 200);

        const char* mime_type = mg_get_mime_type(conn->uri, "octat/stream");
        mg_send_header(conn, "Content-Type", mime_type);

        FILE* fp = fopen(filepath.string().c_str(), "rb");
        if (fp) {
            char buffer[4096];
            size_t bytes_read = 0;
            const size_t buffer_size = sizeof(buffer);
            do {
                bytes_read = fread(buffer, sizeof(char), buffer_size, fp);
                mg_send_data(conn, buffer, bytes_read);
            } while (bytes_read == buffer_size);
            fclose(fp);
        }
    } else {
        mg_send_status(conn, 404);
        mg_send_data(conn, 0, 0);
    }
    return MG_TRUE;
}

v8::Isolate* Device::isolate(void) const {
    return context_.isolate();
}

v8::Handle<v8::Context> Device::context(void) const {
    return context_.context();
}

TemplateFactory& Device::template_factory(void) const {
    return template_factory_;
}

const char* Device::id(void) const {
    return id_.c_str();
}

const char* Device::name(void) const {
    return name_.c_str();
}

const char* Device::model(void) const {
    return model_.c_str();
}

v8::Local<v8::Object> Device::self(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, self_);
}

v8::Local<v8::Object> Device::service_load_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_service_load_);
}

void Device::set_service_load_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    on_service_load_.Reset(isolate, trigger);
}

v8::Local<v8::Object> Device::closed_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_closed_);
}

void Device::set_closed_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    on_closed_.Reset(isolate, trigger);
}



DeviceRef::DeviceRef(void)
    : impl_(0) {
    // nothing
}

DeviceRef::DeviceRef(const IOServiceRef& io_service, const char* basepath) 
    : impl_(0) {

    boost::filesystem::path rootpath(basepath);
    boost::filesystem::path settings = rootpath / "settings.json";

    std::string id;
    std::string static_url;
    boost::filesystem::path script_root(basepath);
    boost::filesystem::path static_root(basepath);
    boost::filesystem::path template_root(basepath);
    boost::filesystem::path document_root(basepath);

    if (boost::filesystem::exists(settings)) {

        std::ifstream fin(settings.string());
        if (!fin.is_open()) {
            // TODO(ghilbut): error handling
            return;
        }

        Json::Value val;
        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(fin, root) || !root.isObject()) {
            // TODO(ghilbut): error handling
            return;
        }

        val = root.get("id", Json::Value(Json::nullValue));
        if (!val.isString()) {
            // TODO(ghilbut): error handling
            return;
        }
        id = val.asString();

        val = root.get("SCRIPT_ROOT", Json::Value(Json::nullValue));
        if (!val.isString()) {
            // TODO(ghilbut): error handling
            return;
        }
        script_root /= val.asString();
        if (!boost::filesystem::exists(script_root)
            || !boost::filesystem::is_directory(script_root)) {
           // TODO(ghilbut): error handling
            return;
        }
        script_root = boost::filesystem::canonical(script_root);

        val = root.get("STATIC_URL", Json::Value(Json::nullValue));
        if (!val.isString()) {
            // TODO(ghilbut): error handling
            return;
        }
        static_url = val.asString();

        val = root.get("STATIC_ROOT", Json::Value(Json::nullValue));
        if (!val.isString()) {
            // TODO(ghilbut): error handling
            return;
        }
        static_root /= val.asString();
        if (!boost::filesystem::exists(static_root)
            || !boost::filesystem::is_directory(static_root)) {
           // TODO(ghilbut): error handling
            return;
        }
        static_root = boost::filesystem::canonical(static_root);

        val = root.get("TEMPLATE_ROOT", Json::Value(Json::nullValue));
        if (!val.isString()) {
            // TODO(ghilbut): error handling
            return;
        }
        template_root /= val.asString();
        if (!boost::filesystem::exists(template_root)
            || !boost::filesystem::is_directory(template_root)) {
           // TODO(ghilbut): error handling
            return;
        }
        template_root = boost::filesystem::canonical(template_root);

        val = root.get("DOCUMENT_ROOT", Json::Value(Json::nullValue));
        if (!val.isString()) {
            // TODO(ghilbut): error handling
            return;
        }
        document_root /= val.asString();
        if (!boost::filesystem::exists(document_root)
            || !boost::filesystem::is_directory(document_root)) {
           // TODO(ghilbut): error handling
            return;
        }
        document_root = boost::filesystem::canonical(document_root);
    }

    impl_ = new Device(io_service
                       , id
                       , rootpath.string()
                       , script_root.string()
                       , static_url
                       , static_root.string()
                       , template_root.string()
                       , document_root.string());
    impl_->AddRef();
}

DeviceRef::DeviceRef(const DeviceRef& other)
    : impl_(0) {
    Reset(other.impl_);
}

DeviceRef::~DeviceRef(void) {
    if (impl_) {
        impl_->Release();
    }
}

DeviceRef& DeviceRef::operator= (const DeviceRef& other) {
    Reset(other.impl_);
    return *this;
}

bool DeviceRef::operator== (const DeviceRef& other) const {
    return (impl_ == other.impl_);
}

bool DeviceRef::operator!= (const DeviceRef& other) const {
    return (impl_ != other.impl_);
}

Device* DeviceRef::operator-> (void) const {
    return impl_;
}

void DeviceRef::Reset(Device* device) {
    if (device) {
        device->AddRef();
    }
    if (impl_) {
        impl_->Release();
    }
    impl_ = device;
}

bool DeviceRef::IsNull(void) const {
    return (impl_ == 0);
}
