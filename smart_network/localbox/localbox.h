#ifndef LOCALBOX_H_
#define LOCALBOX_H_

#include "server_manager.h"
#include "base/io_service_ref.h"
//#include "basebox/root_storage.h"
#include "basebox/service_manager_delegate.h"
#include "basebox/service_ref.h"
#include "frontend/http_server.h"
#include "backend/channel_delegate.h"
#include <v8.h>
#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <map>


enum mg_event;
struct mg_connection;
class DeviceManager;
class ServiceManager;
class NetworkManager;

class LocalBox : public ChannelDelegate
                 , public ServiceManagerDelegate {
public:
    LocalBox(const char* rootpath);
    ~LocalBox(void);

    int HttpRequest(struct mg_connection* conn, enum mg_event ev);

    void RunShell(void);



    // ServiceManagerDelegate
    virtual void OnServiceOpen(const ServiceRef& service);
    virtual void OnServiceClosed(const ServiceRef& service);

    // ChannelDelegate
    virtual void OnChannelOpen(const ChannelRef& channel, const std::string& text);
    virtual void OnChannelRecv(const ChannelRef& channel, const std::string& text);
    virtual void OnChannelClosed(const ChannelRef& channel);


private:
    v8::Isolate* isolate_;
    v8::HandleScope handle_scope_;
    v8::Handle<v8::Context> context_;

    IOServiceRef io_service_;
    ServerManager servers_;

    DeviceManager* device_manager_;
    ServiceManager* service_manager_;
    NetworkManager* net_manager_;
};

#endif  // LOCALBOX_H_
