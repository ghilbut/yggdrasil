#ifndef CHANNEL_H_
#define CHANNEL_H_

#include "chat_message.h"
#include "channel_delegate.h"
#include "boost_lib_fwd.h"
#include "base/io_service_ref.h"
#include "base/ref_object.h"


class ChannelDelegate;

class Channel : public RefObject {
public:
    virtual void Start(void) = 0;
    //virtual void Stop(void) = 0;
    //virtual void DoCommand() const = 0;

    virtual void Deliver(const chat_message& msg) = 0;

    void BindDelegate(ChannelDelegate* delegate);
    void UnbindDelegate(void);

    void FireEventOpen(const std::string& text);
    void FireEventRecv(const std::string& text);
    void FireEventClosed(void);


protected:
    Channel(const IOServiceRef& io_service);
    virtual ~Channel(void) {}

protected:
    IOServiceRef io_service_;
    ChannelDelegate* delegate_;
};

#endif  // CHANNEL_H_
