#ifndef TCP_CHANNEL_H_
#define TCP_CHANNEL_H_

#include "channel.h"
#include "boost_lib_fwd.h"
#include "chat_message.h"
#include <deque>


typedef std::deque<chat_message> chat_message_queue;

class TcpChannel : public Channel {
public:
    explicit TcpChannel(const IOServiceRef& io_service);
    ~TcpChannel(void);

    virtual void Start(void);
    virtual void Deliver(const chat_message& msg);

    TCP::socket& socket();


private:
    void handle_read_header(const boost::system::error_code& error);
    void handle_read_type(const boost::system::error_code& error);
    void handle_read_body(const boost::system::error_code& error);
    void handle_write(const boost::system::error_code& error);


private:
    TCP::socket socket_;
    chat_message read_msg_;
    chat_message_queue write_msgs_;
};

#endif  // TCP_CHANNEL_H_