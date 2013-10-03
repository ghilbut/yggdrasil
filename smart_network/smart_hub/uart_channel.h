#ifndef UART_CHANNEL_H_
#define UART_CHANNEL_H_

#include "channel.h"

class UartChannel : public Channel {
public:
    UartChannel(void);
    ~UartChannel(void);

    // Channel
    virtual bool Start(void);
    virtual void Stop(void);
    virtual void DoCommand() const;

private:
};


#endif  // UART_CHANNEL_H_