#ifndef TCP_CHANNEL_H_
#define TCP_CHANNEL_H_

#include "channel.h"
#include "boost_lib_fwd.h"
#include "chat_message.h"
#include <deque>


typedef std::deque<chat_message> chat_message_queue;

class TcpChannel : 
    public Channel, 
    public boost::enable_shared_from_this<TcpChannel> {

public:
    typedef boost::shared_ptr<TcpChannel> Ptr;

    explicit TcpChannel(boost::asio::io_service& io_service);
    
    void Start(void);
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