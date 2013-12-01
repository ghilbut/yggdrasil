#ifndef SERVICE_PROXY_H_
#define SERVICE_PROXY_H_

#include "http/http_ui_object.h"


class DeviceDesc;
class ServiceDesc;

class ServiceProxy :public Http::UIObject {
public:
    ServiceProxy(const DeviceDesc& device_desc, ServiceDesc& service_desc, uint32_t port);
    ~ServiceProxy(void) {}

    const ServiceDesc& desc(void) const;


protected:
    virtual bool DoExecute(mg_connection* conn
                           , const char* method
                           , const char* query);
    virtual bool DoRequest(mg_connection* conn
                           , const char* method
                           , const char* query);
    virtual bool DoNotify(const std::string& text);


private:
    const DeviceDesc& device_desc_;
    ServiceDesc& service_desc_;
};

#endif  // SERVICE_PROXY_H_