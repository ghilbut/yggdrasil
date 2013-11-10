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
#include "codebase/utility.h"
#include "codebase/chat_message.h"
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <set>
#include <iostream>
#include <cstdlib>


int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Usage: %s <storage path>\n", argv[0]);
        return -1;
    }

    try {
        boost::asio::io_service io_service;

        typedef boost::shared_ptr<Device> DevicePtr;
        std::set<DevicePtr> device_list;

        const char* basepath = argv[1];
        boost::filesystem::path root(basepath);
        boost::filesystem::directory_iterator itr((root / "services").string());
        boost::filesystem::directory_iterator end;
        for (; itr != end; ++itr) {
            const std::string filepath = (itr->path()).string();\

            std::string description;
            ::ReadText(filepath.c_str(), description);
            device_list.insert(new Device(io_service, description));
        }

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

        device_list.clear();
        t.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}