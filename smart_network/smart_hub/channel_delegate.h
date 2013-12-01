#ifndef CHANNEL_DELEGATE_H_
#define CHANNEL_DELEGATE_H_

#include <string>


class Channel;

class ChannelDelegate {
public:
    virtual void OnConnected(const std::string& json, Channel* channel) = 0;
    virtual void OnResponse(const std::string& json) = 0;
    virtual void OnEvent(const std::string& text) = 0;
    virtual void OnDisconnected(void) = 0;


protected:
    ~ChannelDelegate(void) {}
};

#endif  // CHANNEL_DELEGATOR_H_