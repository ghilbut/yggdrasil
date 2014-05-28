#include "ssdp_ethernet_sender.h"


namespace Ssdp {

EthernetSender::EthernetSender(boost::asio::io_service& io_service) 
    : socket_(io_service, UDP::endpoint(UDP::v4(), 0)) {
    socket_.set_option(boost::asio::socket_base::broadcast(true));
}

EthernetSender::~EthernetSender(void) {
    // nothing
}

void EthernetSender::Send(const char* target) const {
    UDP::endpoint endpoint(boost::asio::ip::address_v4::broadcast(), 9432);
    socket_.send_to(boost::asio::buffer(target, strlen(target)), endpoint);
}

}  // namespace Ssdp