#ifndef CTRL_POINT_H_
#define CTRL_POINT_H_

#include "codebase/channel_delegate.h"
#include "service_factory.h"
#include "main_ui_service.h"
#include "main_ui_handler.h"
#include "codebase/http_websocket_ping_scheduler.h"
#include "codebase/device/device_manager.h"
#include "codebase/boost_lib_fwd.h"
#include <map>
#include <set>


namespace Ssdp {
class Scheduler;
}  // namespace Ssdp

class CtrlPoint : public MainUIHandler
                  , public ChannelDelegate {
public:
    CtrlPoint(const std::string& storage_root);
    ~CtrlPoint(void);

    // ChannelDelegate
    virtual void OnConnected(const std::string& json, Channel* channel);
    virtual void OnResponse(const std::string& json);
    virtual void OnEvent(const std::string& text);
    virtual void OnDisconnected(void);

    bool Start(void);
    void Stop(void);


private:
    void thread_main(void);

    // MainUIDelegate
    virtual void handle_get_device_list(std::string& json);

    bool handle_get_common_path(const char* uri, std::string& filepath);
    void handle_disconnected(const std::string& id);


private:
    IOService io_service_;
    boost::thread thread_;

    Ssdp::Scheduler* ssdp_scheduler_;
    // Http::WebsocketPingScheduler ws_ping_scheduler_;
    const std::string common_root_;

    DeviceManager device_manager_;
    ServiceFactory service_factory_;

    MainUIService main_ui_service_;


    std::set<std::string> connecting_list_;
};

#endif  // CTRL_POINT_H_