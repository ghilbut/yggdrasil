#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include "tcp_channel.h"
#include <string>


class ConnectionDelegate;

class TcpServer {
public:
    TcpServer(IOService& io_service, ChannelDelegate* delegate, unsigned short port = 8091);
    ~TcpServer(void);


private:
    void DoListen(void);
    void handle_accept(TcpChannel* channel, const boost::system::error_code& error);


private:
    IOService& io_service_;
    TCP::acceptor acceptor_;
    ChannelDelegate* delegate_;
};


#endif  // TCP_SERVER_H_