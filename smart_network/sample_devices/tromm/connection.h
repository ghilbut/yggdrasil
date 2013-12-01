#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "device_delegator.h"
#include "codebase/chat_message.h"
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <queue>


class DeviceDelegator;

typedef std::deque<chat_message> chat_message_queue;

class Connection {
public:
    typedef boost::function<void (const std::string&)> CloseHandle;


public:
    Connection(boost::asio::io_service& io_service
               , const std::string& description
               , DeviceDelegator& delegator);
    ~Connection(void);

    void Connect(const std::string& address, uint32_t port);
    void Write(const chat_message& msg);
    void Close(void);

    void BindCloseCallback(CloseHandle handle);
    void UnbindCloseCallback(void);

private:
    void handle_connect(const boost::system::error_code& error);
    void handle_read_header(const boost::system::error_code& error);
    void handle_read_type(const boost::system::error_code& error);
    void handle_read_body(const boost::system::error_code& error);
    void handle_write(const boost::system::error_code& error);

    void do_write(chat_message msg);
    void do_close(void);


private:
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::socket socket_;

    std::string address_;
    const std::string& description_;
    chat_message_queue write_msgs_;
    chat_message read_msg_;

    CloseHandle close_callback_;

    DeviceDelegator& delegator_;
};

#endif  // CONNECTION_H_