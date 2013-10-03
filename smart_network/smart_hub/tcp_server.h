#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include "tcp_channel.h"
#include <string>


class TcpServer {
public:
    explicit TcpServer(boost::asio::io_service& io_service, unsigned short port = 8091);
    ~TcpServer(void);

    void BindHandleConnect(Channel::HandleConnected handle);
    void UnbindHandleConnect(void);


private:
    void DoListen(void);
    void handle_accept(TcpChannel::Ptr channel, const boost::system::error_code& error);

private:
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;

    Channel::HandleConnected fire_connected_;
};


#endif  // TCP_SERVER_H_