#ifndef SERVICE_H_
#define SERVICE_H_

#include "channel.h"
#include "http/http_object.h"
#include "codebase/boost_lib_fwd.h"
#include <string>


class DeviceDesc;
class ServiceDesc;
class HttpRequest;

class Service : public Http::Object {
public:
    typedef boost::function<void (const char*)> HandleDisconnected;
        

public:
    Service(const DeviceDesc*& device
            , ServiceDesc*& service
            , uint32_t port);
    ~Service(void);

    void BindChannel(Channel* channel);
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

    const ServiceDesc& info(void) const;


private:
    const DeviceDesc* device_;
    ServiceDesc* service_;
    Channel* channel_;



    typedef std::map<std::string, HttpRequest*> RequestMap;
    RequestMap requests_;
};


#endif  // SERVICE_H_