#ifndef HTTP_SERVER_MANAGER_H_
#define HTTP_SERVER_MANAGER_H_

#include "http_server.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <map>




class HttpServerManager {

public:
    HttpServerManager(void);
    ~HttpServerManager(void);

    void Start(void);
    void Stop(void);

    void thread_main(void);

private:
    boost::thread thread_;
};

#endif  // HTTP_SERVER_MANAGER_H_