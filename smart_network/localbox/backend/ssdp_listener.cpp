#include "ssdp_listener.h"


SsdpListener::SsdpListener(boost::asio::io_service& io_service)
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

void SsdpListener::handle_receive_from(const boost::system::error_code& error, size_t bytes_recvd) {
    if (!error)
    {
        OnSsdp(std::string(data_, data_ + bytes_recvd), sender_endpoint_.address().to_string());
        socket_.async_receive_from(boost::asio::buffer(data_, max_length)
                                   , sender_endpoint_
                                   , boost::bind(&SsdpListener::handle_receive_from, this
                                                 , boost::asio::placeholders::error
                                                 , boost::asio::placeholders::bytes_transferred));
    }
}