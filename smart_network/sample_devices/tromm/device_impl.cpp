#include "device_impl.h"



Device::Impl::Impl(boost::asio::io_service& io_service, const std::string& description)
    : receiver(io_service) // TEST(jh81.kim):
    , io_service_(io_service)
    , interval_(boost::posix_time::seconds(23))
    , timer_(io_service, interval_)
    , description_(description) {

        Json::Value root;
        ::ReadJson(description, root);

        id_ = root["id"].asString();

        timer_.expires_from_now(interval_);
        timer_.async_wait(boost::bind(&Device::Impl::FireEvent, this));
}

Device::Impl::~Impl(void) {
    // nothing
}

void Device::Impl::OnUDP(const std::string& id, const std::string& address) {

    if (id_ != id) {
        return;
    }

    if (connections_.find(address) != connections_.end()) {
        printf("[Warning][OnUDP] %s already connected.\n", address.c_str());
        return;
    }

    Connection* conn = new Connection(io_service_, description_, *this);
    connections_[address] = conn;
    conn->Connect(address, 8070);
}

void Device::Impl::FireEvent(void) {

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
    timer_.async_wait(boost::bind(&Device::Impl::FireEvent, this));
}

void Device::Impl::OnClose(const std::string& address) {
    std::map<std::string, Connection*>::iterator itr = connections_.find(address);
    if (itr != connections_.end()) {
        Connection* conn = itr->second;
        connections_.erase(itr);
        delete conn;
    }
}



void Device::Impl::BindEventConnected(EventConnectedCallback cb) {
    fire_connected = cb;
}

void Device::Impl::BindEventRequest(EventRequestCallback cb) {
    fire_request = cb;
    
}

void Device::Impl::BindEventError(EventErrorCallback cb) {
    fire_error = cb;
    
}

void Device::Impl::BindEventDisconnected(EventDisconnectedCallback cb) {
    fire_disconnected = cb;
}

void Device::Impl::OnConnected(Event& event) {
    if (fire_connected) {
        fire_connected(event);
    }
}

void Device::Impl::OnRequest(const Request& req, Response& res) {
    if (fire_request) {
        fire_request(req, res);
    }
}

void Device::Impl::OnError(void) {    
    if (fire_error) {
        fire_error();
    }
}

void Device::Impl::OnDisconnected(void) {
    if (fire_disconnected) {
        fire_disconnected();
    }
}