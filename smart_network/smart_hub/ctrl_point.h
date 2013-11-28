#ifndef CTRL_POINT_H_
#define CTRL_POINT_H_

#include "channel.h"
#include "http/http_object.h"
#include "service/service_pool.h"
#include "codebase/device/device_pool.h"
#include "codebase/boost_lib_fwd.h"
#include <map>
#include <set>


class HttpServer;
class DevicePool;
class ServicePool;
class SsdpSender;

class CtrlPoint : public HttpObject {
public:
    CtrlPoint(const std::string& document_root
              , HttpServer& httpd
              , DevicePool& devices
              , ServicePool& services);
    ~CtrlPoint(void);

    // HttpServer
    bool Start(void);
    void Stop(void);


private:
    void thread_main(void);

    virtual bool DoExecute(mg_connection* conn
                           , const char* method
                           , const char* uri);
    virtual bool DoRequest(mg_connection* conn
                           , const char* method
                           , const char* query);
    virtual bool DoNotify(const std::string& text);
    

    // channel binding
    void OnConnected(const std::string& json, Channel::Ptr channel);
    void OnDisonnected(const char* id);


private:
    boost::asio::io_service io_service_;
    boost::thread thread_;

    HttpServer& httpd_;
    DevicePool& devices_;
    ServicePool& services_;
    SsdpSender* ssdp_sender_;

    std::set<std::string> connecting_list_;
};


#endif  // CTRL_POINT_H_