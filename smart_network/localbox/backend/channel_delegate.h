#ifndef CHANNEL_DELEGATE_H_
#define CHANNEL_DELEGATE_H_

#include <string>


class ChannelRef;

class ChannelDelegate {
public:
    virtual void OnChannelOpen(const ChannelRef& channel, const std::string& text) = 0;
    virtual void OnChannelRecv(const ChannelRef& channel, const std::string& text) = 0;
    virtual void OnChannelClosed(const ChannelRef& channel) = 0;

protected:
    virtual ~ChannelDelegate(void) {}
};

#endif  // CHANNEL_DELEGATOR_H_