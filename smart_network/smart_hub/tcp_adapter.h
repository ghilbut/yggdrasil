#ifndef TCP_ADAPTER_H_
#define TCP_ADAPTER_H_

#include "tcp_channel.h"
#include <string>


class ConnectionDelegate;

class TcpAdapter {
public:
    TcpAdapter(IOService& io_service, ChannelDelegate* delegate, unsigned short port = 8091);
    ~TcpAdapter(void);


private:
    void DoListen(void);
    void handle_accept(TcpChannel* channel, const boost::system::error_code& error);


private:
    IOService& io_service_;
    TCP::acceptor acceptor_;
    ChannelDelegate* delegate_;
};


#endif  // TCP_ADAPTER_H_