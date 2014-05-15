#ifndef HTTP_SERVER_MANAGER_H_
#define HTTP_SERVER_MANAGER_H_

#include <mongoose.h>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <map>


struct mg_server;

class HttpServerManager {

public:
    HttpServerManager(boost::asio::io_service& io_service, mg_handler_t handler, void* opaque);
    ~HttpServerManager(void);

    void thread_main(void);

private:
    boost::asio::io_service& io_service_;
    boost::thread thread_;

    

    mg_handler_t request_handler_;
    void* opaque_;
};

#endif  // HTTP_SERVER_MANAGER_H_