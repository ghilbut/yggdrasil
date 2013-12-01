#ifndef SERVICE_PROXY_H_
#define SERVICE_PROXY_H_

#include "http/http_ui_object.h"


class DeviceInfo;
class ServiceInfo;

class ServiceProxy :public Http::UIObject {
public:
    ServiceProxy(const DeviceInfo& device_desc, ServiceInfo& service_desc, uint32_t port);
    ~ServiceProxy(void) {}

    const ServiceInfo& desc(void) const;


protected:
    virtual bool DoExecute(mg_connection* conn
                           , const char* method
                           , const char* query);
    virtual bool DoRequest(mg_connection* conn
                           , const char* method
                           , const char* query);
    virtual bool DoNotify(const std::string& text);


private:
    const DeviceInfo& device_desc_;
    ServiceInfo& service_desc_;
};

#endif  // SERVICE_PROXY_H_