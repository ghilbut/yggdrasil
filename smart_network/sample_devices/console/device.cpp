#include "device.h"

#include "codebase/utility.h"
#include <boost/bind.hpp>


Device::Device(boost::asio::io_service& io_service, const std::string& description)
    : receiver(io_service) // TEST(jh81.kim):
    , io_service_(io_service)
    , interval_(boost::posix_time::seconds(23))
    , timer_(io_service, interval_)
    , description_(description) {

    Json::Value root;
    ::ReadJson(description, root);

    id_ = root["id"].asString();

    timer_.expires_from_now(interval_);
    timer_.async_wait(boost::bind(&Device::FireEvent, this));
}

Device::~Device(void) {
    // nothing
}

void Device::FireEvent(void) {

    char text[1024];
    const int size = sprintf(text, "[Device] Evnet [%8.3f]\n", (double)timer_.expires_at().time_of_day().total_milliseconds() / 1000.0);
    printf(text);



    chat_message msg;
    msg.type(chat_message::kEvent);
    msg.body_length(size);
    memcpy(msg.body(), text,size);
    msg.encode_header();
    //write(msg);

    timer_.expires_at(timer_.expires_at() + interval_);
    timer_.async_wait(boost::bind(&Device::FireEvent, this));
}


void Device::OnUDP(const std::string& id, const std::string& address) {

    if (id_ != id) {
        return;
    }

    if (connections_.find(address) != connections_.end()) {
        printf("[Warning][OnUDP] %s already connected.\n", address.c_str());
        return;
    }

    Connection* conn = new Connection(io_service_, description_);
    connections_[address] = conn;
    conn->Connect(address, 8070);
}

void Device::OnClose(const std::string& address) {
    std::map<std::string, Connection*>::iterator itr = connections_.find(address);
    if (itr != connections_.end()) {
        Connection* conn = itr->second;
        connections_.erase(itr);
        delete conn;
    }
}