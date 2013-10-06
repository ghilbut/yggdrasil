#include "service_finder.h"

#include <json/reader.h>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <cstdio>


ServiceFinder::ServiceFinder(boost::asio::io_service& io_service, int port)
    : socket_(io_service, UDP::endpoint(UDP::v4(), 0))
    , interval_(boost::posix_time::seconds(5))
    , timer_(io_service, interval_) {

    socket_.set_option(boost::asio::socket_base::broadcast(true));

    timer_.expires_from_now(interval_);
    timer_.async_wait(boost::bind(&ServiceFinder::handle_find, this));
}

ServiceFinder::~ServiceFinder(void) {

}

void ServiceFinder::RegistTarget(const char* target) {
    boost::mutex::scoped_lock lock(mutex_);
    targets_.insert(target);
}

void ServiceFinder::UnregistTarget(const char* target) {
    boost::mutex::scoped_lock lock(mutex_);
    targets_.erase(target);
}

void ServiceFinder::handle_find(void) {
    mutex_.lock();
    if (!targets_.empty()) {
        UDP::endpoint endpoint(boost::asio::ip::address_v4::broadcast(), 9432);
        std::set<std::string>::const_iterator itr = targets_.begin();
        std::set<std::string>::const_iterator end = targets_.end();
        for (; itr != end; ++itr) {
            const std::string& target = *itr;
            socket_.send_to(boost::asio::buffer(&target[0], target.length()), endpoint);
        }

        printf("[ServiceFinder] find service [%8.3f]\n", (double)timer_.expires_at().time_of_day().total_milliseconds() / 1000.0);
    }
    mutex_.unlock();

    timer_.expires_at(timer_.expires_at() + interval_);
    timer_.async_wait(boost::bind(&ServiceFinder::handle_find, this));
}