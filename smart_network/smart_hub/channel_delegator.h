#ifndef CHANNEL_DELEGATOR_H_
#define CHANNEL_DELEGATOR_H_

#include <string>


class ChannelDelegator {
public:
    virtual void OnEvent(const std::string& text) = 0;
    virtual void OnDisconnected(void) = 0;


private:
    ~ChannelDelegator(void) {}
};

#endif  // CHANNEL_DELEGATOR_H_