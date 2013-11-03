#include "tcp_server.h"

#include "asio_fwd.h"
#include <algorithm>
#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>
#include "tcp_channel.h"


TcpServer::TcpServer(boost::asio::io_service& io_service, unsigned short port) 
        : io_service_(io_service)
        , acceptor_(io_service, Tcp::endpoint(Tcp::v4(), port)) {
        //, acceptor_(io_service, Tcp::endpoint(boost::asio::ip::address::from_string("192.168.1.2"), port)) {
        //, acceptor_(io_service, Tcp::endpoint(boost::asio::ip::address::from_string("192.168.0.4"), port)) {
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    DoListen();
}

TcpServer::~TcpServer(void) {
}

void TcpServer::DoListen(void) {
    TcpChannel::Ptr channel(new TcpChannel(io_service_));

    acceptor_.async_accept(channel->socket()
                           , boost::bind(&TcpServer::handle_accept, this, channel, boost::asio::placeholders::error));
}

void TcpServer::handle_accept(TcpChannel::Ptr channel, const boost::system::error_code& error) {
    if (!error) {
        channel->BindHandleConnected(fire_connected_);
        channel->Start();
    }
    DoListen();
}

void TcpServer::BindHandleConnect(Channel::HandleConnected handle) {
    fire_connected_ = handle;
}

void TcpServer::UnbindHandleConnect(void) {
    fire_connected_ = 0;
}