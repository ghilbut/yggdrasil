#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include "udp_channel.h"
#include "codebase/boost_lib_fwd.h"


class UdpServer {
public:
    explicit UdpServer(boost::asio::io_service& io_service, unsigned short port = 8091);
    ~UdpServer(void);

    void Listen(void);

private:
    void OnAccept(UdpChannel::Ptr channel, const boost::system::error_code& error);

private:
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
};


#endif  // TCP_SERVER_H_