#ifndef DEVICE_DELEGATOR_H_
#define DEVICE_DELEGATOR_H_

#include <boost/scoped_ptr.hpp>


class Event;
class Request;
class Response;

class DeviceDelegator {
public:
    virtual void OnConnected(Event& event) = 0;
    virtual void OnRequest(const Request& req, Response& res) = 0;
    virtual void OnNotify(const std::string& text) = 0;
    virtual void OnError(void) = 0;
    virtual void OnDisconnected(void) = 0;


protected:
    ~DeviceDelegator(void) {};
};

#endif  // DEVICE_DELEGATOR_H_