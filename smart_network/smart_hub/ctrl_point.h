#ifndef CTRL_POINT_H_
#define CTRL_POINT_H_

#include "channel.h"
#include "service_factory.h"
#include "main_ui_service.h"
#include "main_ui_handler.h"
#include "http/http_websocket_ping_scheduler.h"
#include "codebase/device/device_pool.h"
#include "codebase/boost_lib_fwd.h"
#include <map>
#include <set>


class SsdpSender;

class CtrlPoint : public MainUIHandler {
public:
    CtrlPoint(const std::string& storage_root);
    ~CtrlPoint(void);

    // HttpServer
    bool Start(void);
    void Stop(void);


private:
    void thread_main(void);

    // channel binding
    void OnConnected(const std::string& json, Channel::Ptr channel);
    void OnDisonnected(const char* id);

    // MainUIDelegator
    virtual void handle_get_device_list(std::string& json);

    bool HandleCommonPath(const char* uri, std::string& filepath);


private:
    IOService io_service_;
    boost::thread thread_;

    SsdpSender* ssdp_sender_;
    Http::WebsocketPingScheduler ws_ping_scheduler_;
    const std::string common_root_;

    DevicePool device_desc_factory_;
    ServiceFactory service_factory_;

    MainUIService main_ui_service_;


    std::set<std::string> connecting_list_;
};

#endif  // CTRL_POINT_H_