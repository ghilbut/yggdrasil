#include "tcp_channel.h"
#include "codebase/boost_lib_fwd.h"
#include <json/json.h>


TcpChannel::TcpChannel(boost::asio::io_service& io_service)
    : socket_(io_service) {
}

void TcpChannel::Start(void) {
    boost::asio::async_read(socket_
        , boost::asio::buffer(read_msg_.data(), chat_message::header_length)
        , boost::bind(&TcpChannel::handle_read_header, shared_from_this(), boost::asio::placeholders::error));
}

void TcpChannel::Deliver(const chat_message& msg) {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress) {
        boost::asio::async_write(socket_
            , boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length())
            , boost::bind(&TcpChannel::handle_write, shared_from_this(), boost::asio::placeholders::error));
    }
}

TCP::socket& TcpChannel::socket() {
    return socket_;
}


void TcpChannel::handle_read_header(const boost::system::error_code& error) {
    if (!error && read_msg_.decode_header()) {
        boost::asio::async_read(socket_
            , boost::asio::buffer(read_msg_.data() + chat_message::header_length, chat_message::type_length)
            , boost::bind(&TcpChannel::handle_read_type, shared_from_this(), boost::asio::placeholders::error));
    } else {
        // TODO(ghilbut) : disconnected ?
        printf("[Tcp][handle_read_header] error: %s\n", error.message().c_str());
        FireOnDisconnected();
    }
}

void TcpChannel::handle_read_type(const boost::system::error_code& error) {
    if (!error) {
        boost::asio::async_read(socket_
            , boost::asio::buffer(read_msg_.body(), read_msg_.body_length())
            , boost::bind(&TcpChannel::handle_read_body, shared_from_this(), boost::asio::placeholders::error));
    } else {
        // TODO(ghilbut) : disconnected ?
        printf("[Tcp][handle_read_type] error: %s\n", error.message().c_str());
        FireOnDisconnected();
    }
}

void TcpChannel::handle_read_body(const boost::system::error_code& error) {
    if (!error) {
        




        // TODO(ghilbut):
        // below codes are test.

        std::string json(read_msg_.body(), read_msg_.body_length());
        const chat_message::Type type = read_msg_.type();
        if (type == chat_message::kConnect) {
            FireOnConnected(json, shared_from_this());
        } else if (type == chat_message::kResponse) {
            FireOnResponse(json);
        } else if (type == chat_message::kEvent) {
            FireOnEvent(json);
        } else if (type == chat_message::kDisconnect) {
            FireOnDisconnected();
        } else {
            // TODO(ghilbut): invalid action.
            printf("[Tcp] invalid data\n");
        }

        std::string str(read_msg_.body(), read_msg_.body() + read_msg_.body_length());
        printf("[Tcp] XXXXXX %s \n", str.c_str());

        memset(read_msg_.body(), 0, chat_message::max_body_length);






        // Deliver(read_msg_);

        boost::asio::async_read(socket_
            , boost::asio::buffer(read_msg_.data(), chat_message::header_length)
            , boost::bind(&TcpChannel::handle_read_header, shared_from_this(), boost::asio::placeholders::error));
    } else {
        // TODO(ghilbut) : disconnected ?
        printf("[Tcp][handle_read_body] error\n");
        FireOnDisconnected();
    }
}

void TcpChannel::handle_write(const boost::system::error_code& error) {
    if (!error) {
        write_msgs_.pop_front();
        if (!write_msgs_.empty()) {
            boost::asio::async_write(socket_
                , boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length())
                , boost::bind(&TcpChannel::handle_write, shared_from_this(), boost::asio::placeholders::error));
        }
    } else {
        // TODO(ghilbut) : disconnected ?
        printf("[Tcp][handle_write] error\n");
        FireOnDisconnected();
    }
}