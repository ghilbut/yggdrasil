#ifndef CHANNEL_H_
#define CHANNEL_H_

#include "chat_message.h"
#include "channel_delegate.h"
#include "boost_lib_fwd.h"


class ChannelDelegate;

class Channel {
public:
    //virtual bool Start(void) = 0;
    //virtual void Stop(void) = 0;
    //virtual void DoCommand() const = 0;

    virtual void Deliver(const chat_message& msg) = 0;

    void BindDelegate(ChannelDelegate* delegate) {
        BOOST_ASSERT(delegate != 0);
        delegate_ = delegate;
    }

    void UnbindDelegate(void) {
        BOOST_ASSERT(delegate_ != 0);
        delegate_ = 0;
    }

    void FireOnConnected(const std::string& json) {
        BOOST_ASSERT(delegate_ != 0);
        delegate_->OnConnected(json, this);
    }

    void FireOnResponse(const std::string& json) {
        BOOST_ASSERT(delegate_ != 0);
        delegate_->OnResponse(json);
    }

    void FireOnEvent(const std::string& json) {
        BOOST_ASSERT(delegate_ != 0);
        delegate_->OnEvent(json);
    }

    void FireOnDisconnected(void) {
        BOOST_ASSERT(delegate_ != 0);
        delegate_->OnDisconnected();
    }


protected:
    Channel(void) : delegate_(0) {}
    // virtual ~Channel(void) {}


protected:
    ChannelDelegate* delegate_;
};


#endif  // CHANNEL_H_