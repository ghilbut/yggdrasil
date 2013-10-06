#ifndef DEVICE_H_
#define DEVICE_H_

#include <base/chat_message.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <deque>





class receiver
{
public:
    receiver(boost::asio::io_service& io_service)
        : socket_(io_service) {
        // Create the socket so that multiple may be bound to the same address.
        boost::asio::ip::udp::endpoint listen_endpoint(boost::asio::ip::udp::v4(), 9432);
        socket_.open(listen_endpoint.protocol());
        socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
        socket_.bind(listen_endpoint);

        socket_.async_receive_from(
            boost::asio::buffer(data_, max_length), sender_endpoint_,
            boost::bind(&receiver::handle_receive_from, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }

    void handle_receive_from(const boost::system::error_code& error,
        size_t bytes_recvd)
    {
        if (!error)
        {
            std::cout.write(data_, bytes_recvd);
            std::cout << sender_endpoint_.address().to_string();
            std::cout << std::endl;

            OnUDP(std::string(data_, data_ + bytes_recvd), sender_endpoint_.address().to_string());

            socket_.async_receive_from(
                boost::asio::buffer(data_, max_length), sender_endpoint_,
                boost::bind(&receiver::handle_receive_from, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        }
    }

    virtual void OnUDP(const std::string& id, const std::string& address) = 0;


private:
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint sender_endpoint_;
    enum { max_length = 1024 };
    char data_[max_length];
};





typedef std::deque<chat_message> chat_message_queue;

class Device : public receiver
{
public:
    Device(boost::asio::io_service& io_service
           , boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

    void write(const chat_message& msg);
    void close();


private:
    void handle_connect(const boost::system::error_code& error);
    void handle_read_header(const boost::system::error_code& error);
    void handle_read_type(const boost::system::error_code& error);
    void handle_read_body(const boost::system::error_code& error);
    void handle_write(const boost::system::error_code& error);

    void do_write(chat_message msg);
    void do_close(void);


    void FireEvent(void);

    // receiver
    virtual void OnUDP(const std::string& id, const std::string& address);


private:
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::socket socket_;
    chat_message read_msg_;
    chat_message_queue write_msgs_;

    boost::posix_time::time_duration interval_;
    mutable boost::asio::deadline_timer timer_;

    std::string id_;
    std::string address_;
};

#endif  // DEVICE_H_