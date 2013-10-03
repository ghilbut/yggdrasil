#ifndef CTRL_POINT_H_
#define CTRL_POINT_H_

#include <map>
#include <set>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "channel.h"
#include "http/http_object.h"
#include "device/device_pool.h"
#include "service/service_pool.h"


class HttpServer;
class DevicePool;
class ServicePool;

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

    virtual bool DoRequest(mg_connection* conn
                           , const char* method
                           , const char* query);
    virtual bool DoExecute(mg_connection* conn
                           , const char* method
                           , const char* uri);

    // channel binding
    void OnConnected(const std::string& json, Channel::Ptr channel);
    void OnDisonnected(const char* id);


private:
    boost::asio::io_service io_service_;
    boost::thread thread_;

    HttpServer& httpd_;
    DevicePool& devices_;
    ServicePool& services_;

    std::set<std::string> connecting_list_;
};


#endif  // CTRL_POINT_H_