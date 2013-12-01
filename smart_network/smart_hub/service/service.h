#ifndef SERVICE_H_
#define SERVICE_H_

#include "channel.h"
#include "http/http_object.h"
#include "codebase/boost_lib_fwd.h"
#include <string>


class DeviceDesc;
class ServiceInfo;
class HttpRequest;

class Service : public Http::Object {
public:
    typedef boost::function<void (const char*)> HandleDisconnected;
        

public:
    Service(const DeviceDesc*& device
            , ServiceInfo*& service
            , uint32_t port
            , Channel::Ptr channel);
    ~Service(void);

    void BindHandleDisconnected(HandleDisconnected handle);
    void BindChannel(Channel::Ptr& channel);
    void UnbindChannel(void);

    // HttpObject
    virtual bool Service::DoExecute(mg_connection* conn
                                    , const char* method
                                    , const char* uri);
    virtual bool DoRequest(mg_connection* conn
                                    , const char* method
                                    , const char* uri);
    virtual bool DoNotify(const std::string& text);

    // channel binding
    void OnResponse(const std::string& json);
    void OnEvent(const std::string& json);
    void OnDisconnected(void);

    const ServiceInfo& info(void) const;


private:
    const DeviceDesc* device_;
    ServiceInfo* service_;
    Channel::Ptr channel_;

    HandleDisconnected fire_disconnected_;



    typedef std::map<std::string, HttpRequest*> RequestMap;
    RequestMap requests_;
};


#endif  // SERVICE_H_