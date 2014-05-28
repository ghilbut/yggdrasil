#include "tcp_adapter.h"
#include "base/io_service.h"
#include <algorithm>


TcpAdapter::TcpAdapter(const IOServiceRef& io_service, ChannelDelegate* delegate, unsigned short port) 
        : io_service_(io_service)
        , acceptor_(io_service->IO(), TCP::endpoint(TCP::v4(), port)) 
        , delegate_(delegate)
        //, acceptor_(io_service, TCP::endpoint(boost::asio::ip::address::from_string("192.168.1.2"), port)) {
        //, acceptor_(io_service, TCP::endpoint(boost::asio::ip::address::from_string("192.168.0.4"), port)) {
        , ssdp_sender_(io_service->IO()) {
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    DoListen();
}

TcpAdapter::~TcpAdapter(void) {
    // nothing
}

void TcpAdapter::SendSsdp(const char* target) const {
    ssdp_sender_.Send(target);
}



void TcpAdapter::DoListen(void) {
    TcpChannel* channel = new TcpChannel(io_service_);
    channel->AddRef();

    acceptor_.async_accept(channel->socket()
                           , boost::bind(&TcpAdapter::handle_accept, this, channel, boost::asio::placeholders::error));
}

void TcpAdapter::handle_accept(Channel* channel, const boost::system::error_code& error) {
    if (error) {
        channel->Release();
    } else {
        channel->BindDelegate(delegate_);
        channel->Start();
    }
    DoListen();
}
