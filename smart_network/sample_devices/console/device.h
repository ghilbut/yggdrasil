#ifndef DEVICE_H_
#define DEVICE_H_

#include <base/chat_message.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>
#include <deque>


typedef std::deque<chat_message> chat_message_queue;

class Device
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


private:
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::socket socket_;
    chat_message read_msg_;
    chat_message_queue write_msgs_;

    boost::posix_time::time_duration interval_;
    mutable boost::asio::deadline_timer timer_;
};

#endif  // DEVICE_H_