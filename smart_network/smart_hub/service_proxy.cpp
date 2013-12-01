#include "service_proxy.h"
#include "codebase/device/device_desc.h"
#include "service/service_desc.h"


ServiceProxy::ServiceProxy(const DeviceDesc& device_desc, ServiceDesc& service_desc, uint32_t port)
    : Http::UIObject(device_desc.uiroot(), port) 
    , device_desc_(device_desc)
    , service_desc_(service_desc) {
    // nothing
}

const ServiceDesc& ServiceProxy::desc(void) const {
    return service_desc_;
}

bool ServiceProxy::DoExecute(mg_connection* conn
                             , const char* method
                             , const char* query) {
    return false;
}

bool ServiceProxy::DoRequest(mg_connection* conn
                             , const char* method
                             , const char* query) {
    return false;
}

bool ServiceProxy::DoNotify(const std::string& text) {
    return false;
}