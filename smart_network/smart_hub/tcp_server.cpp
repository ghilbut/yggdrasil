#include "tcp_server.h"
#include <algorithm>


TcpServer::TcpServer(IOService& io_service, ChannelDelegate* delegate, unsigned short port) 
        : io_service_(io_service)
        , acceptor_(io_service, TCP::endpoint(TCP::v4(), port)) 
        , delegate_(delegate) {
        //, acceptor_(io_service, TCP::endpoint(boost::asio::ip::address::from_string("192.168.1.2"), port)) {
        //, acceptor_(io_service, TCP::endpoint(boost::asio::ip::address::from_string("192.168.0.4"), port)) {
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    DoListen();
}

TcpServer::~TcpServer(void) {
    // nothing
}



void TcpServer::DoListen(void) {
    TcpChannel* channel = new TcpChannel(io_service_);
    channel->BindDelegate(delegate_);

    acceptor_.async_accept(channel->socket()
                           , boost::bind(&TcpServer::handle_accept, this, channel, boost::asio::placeholders::error));
}

void TcpServer::handle_accept(TcpChannel* channel, const boost::system::error_code& error) {
    if (!error) {
        channel->Start();
    }
    DoListen();
}