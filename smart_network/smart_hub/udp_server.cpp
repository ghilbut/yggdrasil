#include "udp_server.h"
#include <algorithm>
#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>


UdpServer::UdpServer(boost::asio::io_service& io_service, unsigned short port) 
        : io_service_(io_service)
        , acceptor_(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)) {
    Listen();
}

UdpServer::~UdpServer(void) {
}

void UdpServer::Listen(void) {
    UdpChannel::Ptr channel(new UdpChannel(io_service_));

    acceptor_.async_accept(
        channel->socket(),
        boost::bind(&UdpServer::OnAccept, this, channel, boost::asio::placeholders::error)
        );
}

void UdpServer::OnAccept(UdpChannel::Ptr channel, const boost::system::error_code& error) {
    if (!error) {
        channel->Start();
    }
    Listen();
}