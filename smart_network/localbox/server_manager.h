#ifndef SERVER_MANAGER_H_
#define SERVER_MANAGER_H_

#include "frontend/http_server.h"
#include <boost/atomic.hpp>
#include <boost/thread.hpp>


enum mg_event;
struct mg_connection;

class ServerManager {
public:
    ServerManager(void);
    ~ServerManager(void);

    typedef boost::function<int (struct mg_connection*, enum mg_event)> RequestHandle;
    void Create(int port, RequestHandle handler);
    void Destroy(int port);

    int HandleRequest(mg_connection* conn, mg_event ev);


private:
    void thread_main(void);


private:
    boost::mutex mutex_;
    boost::thread thread_;
    boost::atomic_bool alive_;
    boost::atomic_bool stop_;

    typedef std::map<int, Http::Server*> ServerMap;
    typedef std::map<int, RequestHandle> RequestHandleMap;
    ServerMap updates_;
    ServerMap servers_;
    RequestHandleMap request_handles_;
};

#endif  // SERVER_MANAGER_H_