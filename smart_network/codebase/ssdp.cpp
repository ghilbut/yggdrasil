#include "ssdp.h"


SsdpSender::SsdpSender(IOService& io_service)
    : socket_(io_service, UDP::endpoint(UDP::v4(), 0))
    , interval_(boost::posix_time::seconds(5))
    , timer_(io_service, interval_) {

        socket_.set_option(boost::asio::socket_base::broadcast(true));

        timer_.expires_from_now(interval_);
        timer_.async_wait(boost::bind(&SsdpSender::handle_send, this));
}

SsdpSender::~SsdpSender(void) {
    // nothing
}

void SsdpSender::RegistTarget(const char* target) {
    boost::mutex::scoped_lock lock(mutex_);
    targets_.insert(target);
}

void SsdpSender::UnregistTarget(const char* target) {
    boost::mutex::scoped_lock lock(mutex_);
    targets_.erase(target);
}

void SsdpSender::handle_send(void) {
    const double time = (double)timer_.expires_at().time_of_day().total_milliseconds() / 1000.0;    
    if (!targets_.empty()) {
        printf("[INFO][SsdpSender] search service(s). [%8.3f]\n", time);
        UDP::endpoint endpoint(boost::asio::ip::address_v4::broadcast(), 9432);

        boost::unique_lock<boost::mutex> lock(mutex_);
        std::set<std::string>::const_iterator itr = targets_.begin();
        std::set<std::string>::const_iterator end = targets_.end();
        for (; itr != end; ++itr) {
            const std::string& target = *itr;
            socket_.send_to(boost::asio::buffer(&target[0], target.length()), endpoint);
            printf("  - %s\n", target.c_str());
        }
    } else {
        printf("[INFO][SsdpSender] there is no search service. [%8.3f]\n", time);
    }
    printf("\n");

    timer_.expires_at(timer_.expires_at() + interval_);
    timer_.async_wait(boost::bind(&SsdpSender::handle_send, this));
}



SsdpListener::SsdpListener(IOService& io_service)
    : socket_(io_service) {
        // Create the socket so that multiple may be bound to the same address.
        UDP::endpoint listen_endpoint(UDP::v4(), 9432);
        socket_.open(listen_endpoint.protocol());
        socket_.set_option(UDP::socket::reuse_address(true));
        socket_.bind(listen_endpoint);

        socket_.async_receive_from(boost::asio::buffer(data_, max_length)
                                   , sender_endpoint_
                                   , boost::bind(&SsdpListener::handle_receive_from, this
                                                 , boost::asio::placeholders::error
                                                 , boost::asio::placeholders::bytes_transferred));
}

SsdpListener::~SsdpListener(void) {
    // nothing
}

void SsdpListener::handle_receive_from(const boost::system::error_code& error,
    size_t bytes_recvd)
{
    if (!error)
    {
        std::cout.write(data_, bytes_recvd);
        std::cout << sender_endpoint_.address().to_string();
        std::cout << std::endl;

        OnSsdp(std::string(data_, data_ + bytes_recvd), sender_endpoint_.address().to_string());

        socket_.async_receive_from(boost::asio::buffer(data_, max_length)
                                   , sender_endpoint_
                                   , boost::bind(&SsdpListener::handle_receive_from, this
                                                 , boost::asio::placeholders::error
                                                 , boost::asio::placeholders::bytes_transferred));
    }
}