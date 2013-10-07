//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "device.h"
#include "base/chat_message.h"
#include <boost/thread/thread.hpp>
#include <iostream>
#include <cstdlib>



int main(int argc, char* argv[])
{
    try
    {
        boost::asio::io_service io_service;

        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query("127.0.0.1", "8070");
        //boost::asio::ip::tcp::resolver::query query("192.168.1.2", "8070");
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

        Device device(io_service, iterator);

        boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

        char line[chat_message::max_body_length + 1];
        while (std::cin.getline(line, chat_message::max_body_length + 1))
        {
            using namespace std; // For strlen and memcpy.
            chat_message msg;
            msg.body_length(strlen(line));
            memcpy(msg.body(), line, msg.body_length());
            msg.encode_header();
            //device.write(msg);
        }

        //device.close();
        t.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}