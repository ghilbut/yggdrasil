#include "ssdp_ethernet_sender.h"


namespace Ssdp {

EthernetSender::EthernetSender(IOService& io_service) 
    : socket_(io_service, UDP::endpoint(UDP::v4(), 0)) {
    socket_.set_option(boost::asio::socket_base::broadcast(true));
}

EthernetSender::~EthernetSender(void) {
    // nothing
}

void EthernetSender::RegistTarget(const std::string& target) {
    boost::mutex::scoped_lock lock(mutex_);
    targets_.insert(target);
}

void EthernetSender::UnregistTarget(const std::string& target) {
    boost::mutex::scoped_lock lock(mutex_);
    targets_.erase(target);
}

void EthernetSender::Send(void) const {
    if (!targets_.empty()) {
        UDP::endpoint endpoint(boost::asio::ip::address_v4::broadcast(), 9432);
        boost::unique_lock<boost::mutex> lock(mutex_);
        std::set<std::string>::const_iterator itr = targets_.begin();
        std::set<std::string>::const_iterator end = targets_.end();
        for (; itr != end; ++itr) {
            const std::string& target = *itr;
            socket_.send_to(boost::asio::buffer(&target[0], target.length()), endpoint);
        }
    }
}

}  // namespace Ssdp