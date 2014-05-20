#ifndef IO_SERVICE_H_
#define IO_SERVICE_H_

#include "ref_implement.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>


class IOService : public RefImplement {
public:
    IOService(void);
    ~IOService(void);

    template <typename F>
    inline void Post(const F& handler) {
        strand_.post(handler);
    }

private:
    boost::asio::io_service io_service_;
    boost::asio::io_service::work* work_;
    boost::asio::strand strand_;
    boost::thread thread_;
};

#endif  // IO_SERVICE_H_