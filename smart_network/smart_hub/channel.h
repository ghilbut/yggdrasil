#ifndef CHANNEL_H_
#define CHANNEL_H_

#include "base/chat_message.h"
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>


class Channel {
public:
    typedef boost::shared_ptr<Channel> Ptr;
    typedef boost::function<void (const std::string&, Channel::Ptr channel)> HandleConnected;
    typedef boost::function<void (const std::string&)> HandleResponse;
    typedef boost::function<void (const std::string&)> HandleEvent;
    typedef boost::function<void ()>                   HandleDisconnected;

    //virtual bool Start(void) = 0;
    //virtual void Stop(void) = 0;
    //virtual void DoCommand() const = 0;

    void BindHandleConnected(HandleConnected handle) {
        fire_connected_ = handle;
    }

    void BindHandleResponse(HandleResponse handle) {
        fire_response_ = handle;
    }

    void BindHandleEvent(HandleEvent handle) {
        fire_event_ = handle;
    }

    void BindHandleDisconnected(HandleDisconnected handle) {
        fire_disconnected_ = handle;
    }

protected:
    void FireOnConnected(const std::string& json, Channel::Ptr channel) {
        if (fire_connected_) {
            fire_connected_(json, channel);
        }
    }

    void FireOnResponse(const std::string& json) {
        if (fire_response_) {
            fire_response_(json);
        }
    }

    void FireOnEvent(const std::string& json) {
        if (fire_event_) {
            fire_event_(json);
        }
    }

    void FireOnDisconnected(void) {
        if (fire_disconnected_) {
            fire_disconnected_();
        }
    }


protected:
    ~Channel(void) {}

private:
    HandleConnected    fire_connected_;
    HandleResponse     fire_response_;
    HandleEvent        fire_event_;
    HandleDisconnected fire_disconnected_;
};


#endif  // CHANNEL_H_