#ifndef DEVICE_H_
#define DEVICE_H_

#include <boost/asio.hpp>


class Event;
class Request;
class Response;

class Device
{
public:
    Device(boost::asio::io_service& io_service, const std::string& description);
    virtual ~Device(void);

    void FireEvent(const std::string& json);
    void OnClose(const std::string& address);


    virtual void OnConnected(Event& event) = 0;
    virtual void OnRequest(const Request& req, Response& res) = 0;
    virtual void OnNotify(const std::string& text) = 0;
    virtual void OnError(void) = 0;
    virtual void OnDisconnected(void) = 0;


private:
    class Impl;
    Impl* impl_;
};

#endif  // DEVICE_H_