#ifndef DEVICE_IMPL_H_
#define DEVICE_IMPL_H_

#include "device.h"
#include "connection.h"
#include "device_delegator.h"
#include "codebase/utility.h"
#include "codebase/chat_message.h"
#include <json/json.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <deque>


class receiver
{
public:
    receiver(boost::asio::io_service& io_service)
        : socket_(io_service) {
            // Create the socket so that multiple may be bound to the same address.
            boost::asio::ip::udp::endpoint listen_endpoint(boost::asio::ip::udp::v4(), 9432);
            socket_.open(listen_endpoint.protocol());
            socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
            socket_.bind(listen_endpoint);

            socket_.async_receive_from(
                boost::asio::buffer(data_, max_length), sender_endpoint_,
                boost::bind(&receiver::handle_receive_from, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    void handle_receive_from(const boost::system::error_code& error,
        size_t bytes_recvd)
    {
        if (!error)
        {
            std::cout.write(data_, bytes_recvd);
            std::cout << sender_endpoint_.address().to_string();
            std::cout << std::endl;

            OnUDP(std::string(data_, data_ + bytes_recvd), sender_endpoint_.address().to_string());

            socket_.async_receive_from(
                boost::asio::buffer(data_, max_length), sender_endpoint_,
                boost::bind(&receiver::handle_receive_from, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        }
    }

    virtual void OnUDP(const std::string& id, const std::string& address) = 0;


private:
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint sender_endpoint_;
    enum { max_length = 1024 };
    char data_[max_length];
};



class Device::Impl : public receiver, public DeviceDelegator {
public:
    Impl(boost::asio::io_service& io_service, const std::string& description);
    ~Impl(void);

    // receiver
    virtual void OnUDP(const std::string& id, const std::string& address);

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