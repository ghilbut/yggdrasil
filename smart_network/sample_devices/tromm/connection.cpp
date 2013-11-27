#include "connection.h"

#include "message.h"
#include <boost/bind.hpp>


Connection::Connection(boost::asio::io_service& io_service
                       , const std::string& description
                       , DeviceDelegator& delegator) 
    : io_service_(io_service)
    , socket_(io_service)
    , description_(description)
    , delegator_(delegator) {
}

Connection::~Connection(void) {
}

void Connection::Connect(const std::string& address, uint32_t port) {

    char sport[10];
    const int size = sprintf(sport, "%u", port);
    if (size < 0) {
        printf("[Error][Connection::Connect] invalid port value.\n");
        // TODO(jh81.kim): error return ?
        return;
    }

    boost::asio::ip::tcp::resolver resolver(io_service_);
    boost::asio::ip::tcp::resolver::query query(address, sport);
    boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

    boost::asio::async_connect(socket_
        , iterator
        , boost::bind(&Connection::handle_connect, this, boost::asio::placeholders::error));

    printf("[Info][Connection::Connect] %s:%s\n", address.c_str(), sport);
}

void Connection::Write(const chat_message& msg) {
    io_service_.post(boost::bind(&Connection::do_write, this, msg));
}

void Connection::Close(void) {
    io_service_.post(boost::bind(&Connection::do_close, this));
}

void Connection::BindCloseCallback(CloseHandle handle) {
    close_callback_ = handle;
}

void Connection::UnbindCloseCallback(void) {
    close_callback_ = 0;
}

void Connection::handle_connect(const boost::system::error_code& error) {
    if (error) {
        printf("[Error][Connection::handle_connect] %s\n", error.message().c_str());
        return;
    }



    const size_t size = description_.length();

    chat_message chat;
    chat.type(chat_message::kConnect);
    chat.body_length(size);
    strncpy(chat.body(), description_.c_str(), size);
    chat.encode_header();
    Write(chat);



    boost::asio::async_read(socket_
        , boost::asio::buffer(read_msg_.data(), chat_message::header_length)
        , boost::bind(&Connection::handle_read_header, this, boost::asio::placeholders::error));
}

void Connection::handle_read_header(const boost::system::error_code& error) {
    if (error) {
        printf("[Error][Connection::handle_read_header] %s\n", error.message().c_str());
        do_close();
        return;
    }

    if (!read_msg_.decode_header()) {
        printf("[Error][Connection::handle_read_header] decode header failed\n");
        return;
    }

    boost::asio::async_read(socket_
        , boost::asio::buffer(read_msg_.data() + chat_message::header_length, chat_message::type_length)
        , boost::bind(&Connection::handle_read_type, this, boost::asio::placeholders::error));

    /*boost::asio::async_read(socket_
        , boost::asio::buffer(read_msg_.body() + chat_message::header_length, chat_message::type_length)
        , boost::bind(&Connection::handle_read_body, this, boost::asio::placeholders::error));*/
}

void Connection::handle_read_type(const boost::system::error_code& error) {
    if (error) {
        printf("[Error][Connection::handle_read_type] %s\n", error.message().c_str());
        do_close();
        return;
    }

    if (!read_msg_.decode_header()) {
        printf("[Error][Connection::handle_read_type] decode header failed\n");
        return;
    }

    boost::asio::async_read(socket_
        , boost::asio::buffer(read_msg_.body(), read_msg_.body_length())
        , boost::bind(&Connection::handle_read_body, this, boost::asio::placeholders::error));
}

void Connection::handle_read_body(const boost::system::error_code& error) {
    if (error) {
        printf("[Error][Connection::handle_read_body] %s\n", error.message().c_str());
        do_close();
        return;
    }

    const char* body = read_msg_.body();
    const std::string json(body, body + read_msg_.body_length());

    const chat_message::Type type = read_msg_.type();
    if (type == chat_message::kRequest) {
        printf("[Connection] [Request] %s\n", json.c_str());

        Request::Ptr req(Request::FromJson(json));
        if (req.get() != 0) {
            Response res(*req);
            delegator_.OnRequest(*req, res);

            std::string res_json;
            res.ToJson(res_json);

            chat_message msg;
            const size_t size = res_json.length();
            msg.body_length(size);
            strcpy(msg.body(), res_json.c_str());
            msg.encode_header();
            msg.type(chat_message::kResponse);
            Write(msg);
        }
    } else if (type == chat_message::kNotify) {
        delegator_.OnNotify(json);
    } else {
        // nothing: invalid type
    }

    boost::asio::async_read(socket_
        , boost::asio::buffer(read_msg_.data(), chat_message::header_length)
        , boost::bind(&Connection::handle_read_header, this, boost::asio::placeholders::error));
}

void Connection::handle_write(const boost::system::error_code& error) {
    if (error) {
        printf("[Error][Connection::handle_write] %s\n", error.message().c_str());
        do_close();
        return;
    }

    write_msgs_.pop_front();
    if (write_msgs_.empty()) {
        return;
    }

    boost::asio::async_write(socket_
        , boost::asio::buffer(write_msgs_.front().data()
        , write_msgs_.front().length())
        , boost::bind(&Connection::handle_write, this, boost::asio::placeholders::error));
    
}

void Connection::do_write(chat_message msg) {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress) {
        boost::asio::async_write(socket_
            , boost::asio::buffer(write_msgs_.front().data()
            , write_msgs_.front().length())
            , boost::bind(&Connection::handle_write, this, boost::asio::placeholders::error));
    }
}

void Connection::do_close(void) {
    socket_.close();

    if (close_callback_) {
        close_callback_(socket_.remote_endpoint().address().to_string());
    }
}