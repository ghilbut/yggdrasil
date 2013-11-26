#include "tromm.h"
#include "codebase/utility.h"
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <cstdio>


int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Usage: %s <storage path>\n", argv[0]);
        return -1;
    }

    boost::asio::io_service io_service;

    const char* basepath = argv[1];
    boost::filesystem::path root(basepath);
    boost::filesystem::path path(root / "services/b.txt");

    std::string description;
    ::ReadText(path.string().c_str(), description);

    typedef boost::shared_ptr<Tromm> TrommPtr;
    TrommPtr device(new Tromm(io_service, description));

    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

    char line[1024];
    while (std::cin.getline(line, 1024)) {
        //using namespace std; // For strlen and memcpy.
        //chat_message msg;
        //msg.body_length(strlen(line));
        //memcpy(msg.body(), line, msg.body_length());
        //msg.encode_header();
        //device.write(msg);
    }
    t.join();




    return 0;
}