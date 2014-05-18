#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include <mongoose.h>
#include <boost/atomic.hpp>


namespace Http {

class Server {
public:
    Server(void);
    ~Server(void);

    bool Create(void* server_param, mg_handler_t handler, int port);
    void Destroy(void);
    int Poll(int milliseconds);

private:
    boost::atomic<mg_server*> server_;
};

}  // namespace Http

#endif  // HTTP_SERVER_H_