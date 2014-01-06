#ifndef DEVICE_IMPL_H_
#define DEVICE_IMPL_H_

#include "device.h"
#include "connection.h"
#include "device_delegator.h"
#include "codebase/utility.h"
#include "codebase/chat_message.h"
#include "codebase/ssdp_listener.h"
#include <json/json.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <deque>






class Device::Impl : public SsdpListener, public DeviceDelegator {
public:
    Impl(boost::asio::io_service& io_service, const std::string& description);
    ~Impl(void);

    // receiver
    virtual void OnSsdp(const std::string& ssdp, const std::string& address);

    void FireEvent(const std::string& json);
    void OnClose(const std::string& address);


    typedef boost::function<void (Event&)> EventConnectedCallback;
    typedef boost::function<void (const Request&, Response&)> EventRequestCallback;
    typedef boost::function<void (const std::string&)> EventNotifyCallback;
    typedef boost::function<void ()> EventErrorCallback;
    typedef boost::function<void ()> EventDisconnectedCallback;
    void BindEventConnected(EventConnectedCallback cb);
    void BindEventRequest(EventRequestCallback cb);
    void BindEventNotify(EventNotifyCallback cb);
    void BindEventError(EventErrorCallback cb);
    void BindEventDisconnected(EventDisconnectedCallback cb);

    virtual void OnConnected(Event& event);
    virtual void OnRequest(const Request& req, Response& res);
    virtual void OnNotify(const std::string& text);
    virtual void OnError(void);
    virtual void OnDisconnected(void);



private:
    boost::asio::io_service& io_service_;
    boost::posix_time::time_duration interval_;
    mutable boost::asio::deadline_timer timer_;

    std::map<std::string, Connection*> connections_;
    std::string description_;

    std::string id_;


    EventConnectedCallback fire_connected_;
    EventRequestCallback fire_request_;
    EventNotifyCallback fire_notify_;
    EventErrorCallback fire_error_;
    EventDisconnectedCallback fire_disconnected_;
};

#endif  // DEVICE_IMPL_H_