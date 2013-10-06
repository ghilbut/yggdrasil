#include "device.h"

#include <json/json.h>
#include <boost/bind.hpp>


Device::Device(boost::asio::io_service& io_service
               , boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
    : receiver(io_service) // TEST(jh81.kim):
    , io_service_(io_service)
    , socket_(io_service)
    , interval_(boost::posix_time::seconds(23))
    , timer_(io_service_, interval_) {
    
    /*
    boost::asio::async_connect(socket_
                               , endpoint_iterator
                               , boost::bind(&Device::handle_connect, this, boost::asio::placeholders::error));
    */
    
   



    //const char* path = "E:\\workbench\\project\\yggdrasil\\smart_network\\sample_storage\\services\\a.txt";
    const char* path = "C:\\work\\project\\yggdrasil\\smart_network\\sample_storage\\services\\a.txt";
    FILE* f = fopen(path, "r");
    if (!f) {
        printf("couldn't read below file.\n%s\n", path);
    }

    char buf[1024];
    const size_t size = fread(buf, 1, 1024, f);
    fclose(f);

    Json::Value root(Json::objectValue);
    Json::Reader reader;
    reader.parse(buf, root);
    id_ = root["id"].asString();



    timer_.expires_from_now(interval_);
    timer_.async_wait(boost::bind(&Device::FireEvent, this));
}

void Device::write(const chat_message& msg) {
    io_service_.post(boost::bind(&Device::do_write, this, msg));
}

void Device::close() {
    io_service_.post(boost::bind(&Device::do_close, this));
}

void Device::handle_connect(const boost::system::error_code& error) {
    if (!error) {





        //const char* path = "E:\\workbench\\project\\yggdrasil\\smart_network\\sample_storage\\services\\a.txt";
        const char* path = "C:\\work\\project\\yggdrasil\\smart_network\\sample_storage\\services\\a.txt";
        FILE* f = fopen(path, "r");
        if (!f) {
            printf("couldn't read below file.\n%s\n", path);
        }

        char buf[1024];
        const size_t size = fread(buf, 1, 1024, f);
        fclose(f);

        chat_message chat;
        chat.type(chat_message::kConnect);
        chat.body_length(size);
        strncpy(chat.body(), buf, size);
        chat.encode_header();
        write(chat);

        





        boost::asio::async_read(socket_
                                , boost::asio::buffer(read_msg_.data(), chat_message::header_length)
                                , boost::bind(&Device::handle_read_header, this, boost::asio::placeholders::error));
    } else {
        // TODO(jh81.kim) : disconnected ?
        printf("[handle_connect] error: %s\n", error.message().c_str());
    }
}

void Device::handle_read_header(const boost::system::error_code& error) {
    if (!error && read_msg_.decode_header()) {
        boost::asio::async_read(socket_
                                , boost::asio::buffer(read_msg_.body() + chat_message::header_length, chat_message::type_length)
                                , boost::bind(&Device::handle_read_body, this, boost::asio::placeholders::error));
    } else {
        do_close();
    }
}

void Device::handle_read_type(const boost::system::error_code& error) {
    if (!error && read_msg_.decode_header()) {
        boost::asio::async_read(socket_
                                , boost::asio::buffer(read_msg_.body(), read_msg_.body_length())
                                , boost::bind(&Device::handle_read_body, this, boost::asio::placeholders::error));
    } else {
        do_close();
    }
}

void Device::handle_read_body(const boost::system::error_code& error) {
    if (!error) {
        const char* body = read_msg_.body();
        const std::string json(body, body + read_msg_.body_length());

        const chat_message::Type type = read_msg_.type();
        if (type == chat_message::kCommand) {
            printf("[Device] [Action] %s\n", json.c_str());
        } else if (type == chat_message::kRequest) {
            printf("[Device] [Request] %s\n", json.c_str());
        } else {
            // nothing: invalid type
        }

        boost::asio::async_read(socket_
                                , boost::asio::buffer(read_msg_.data(), chat_message::header_length)
                                , boost::bind(&Device::handle_read_header, this, boost::asio::placeholders::error));
    } else {
        do_close();
    }
}

void Device::handle_write(const boost::system::error_code& error) {
    if (!error) {
        write_msgs_.pop_front();
        if (!write_msgs_.empty()) {
            boost::asio::async_write(socket_
                                     , boost::asio::buffer(write_msgs_.front().data()
                                     , write_msgs_.front().length())
                                     , boost::bind(&Device::handle_write, this, boost::asio::placeholders::error));
        }
    } else {
        do_close();
    }
}

void Device::do_write(chat_message msg) {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress) {
        boost::asio::async_write(socket_
                                 , boost::asio::buffer(write_msgs_.front().data()
                                 , write_msgs_.front().length())
                                 , boost::bind(&Device::handle_write, this, boost::asio::placeholders::error));
    }
}

void Device::do_close(void) {
    socket_.close();
}

void Device::FireEvent(void) {

    char text[1024];
    const int size = sprintf(text, "[Device] Evnet [%8.3f]\n", (double)timer_.expires_at().time_of_day().total_milliseconds() / 1000.0);
    printf(text);



    chat_message msg;
    msg.type(chat_message::kEvent);
    msg.body_length(size);
    memcpy(msg.body(), text,size);
    msg.encode_header();
    write(msg);

    timer_.expires_at(timer_.expires_at() + interval_);
    timer_.async_wait(boost::bind(&Device::FireEvent, this));
}


void Device::OnUDP(const std::string& id, const std::string& address) {

    boost::asio::ip::tcp::resolver resolver(io_service_);
    boost::asio::ip::tcp::resolver::query query(address, "8070");
    boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

    if (id_ == id && address_ != address) {
        address_ = address;

        boost::asio::async_connect(socket_
            , iterator
            , boost::bind(&Device::handle_connect, this, boost::asio::placeholders::error));

        printf("[test] %s\n", socket_.remote_endpoint().address().to_string().c_str());
    }
}