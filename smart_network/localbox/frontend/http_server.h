#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include "environ.h"
#include <mongoose.h>
#include <boost/atomic.hpp>


namespace Http {

class Server {
public:
    Server(void);
    ~Server(void);

    bool Create(void* server_param, mg_handler_t handler, int port);
    void Destroy(void);

    void Pause(void);
    void Resume(void);
    int Poll(int milliseconds);

private:
    mg_server* server_;
    boost::atomic_bool paused_;
};

}  // namespace Http

#endif  // HTTP_SERVER_H_