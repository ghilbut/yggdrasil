#ifndef SERVICE_BROKER_H_
#define SERVICE_BROKER_H_

#include "codebase/channel_delegate.h"
#include "codebase/http_ui_object.h"
#include <map>


class DeviceDesc;
class ServiceDesc;
class Channel;
class HttpRequest;

class ServiceBroker :public Http::UIObject
                    , public ChannelDelegate {
public:
    typedef boost::function<void (const char*)> HandleDisconnected;


public:
    ServiceBroker(const DeviceDesc& device_desc, ServiceDesc& service_desc, uint32_t port);
    ~ServiceBroker(void) {}

    // ChannelDelegate
    virtual void OnConnected(const std::string& json, Channel* channel);
    virtual void OnResponse(const std::string& json);
    virtual void OnEvent(const std::string& text);
    virtual void OnDisconnected(void);

    void BindChannel(Channel* channel);
    void BindDisconnectedHandler(HandleDisconnected handle);

    const ServiceDesc& desc(void) const;


protected:
    virtual bool FireRequest(mg_connection* conn
                             , const char* method
                             , const char* query);
    virtual bool FireNotify(const std::string& text);


private:
    const DeviceDesc& device_desc_;
    ServiceDesc& service_desc_;

    typedef std::map<std::string, HttpRequest*> RequestMap;
    RequestMap requests_;
    Channel* channel_;

    HandleDisconnected handle_disconnected_;
};

#endif  // SERVICE_BROKER_H_