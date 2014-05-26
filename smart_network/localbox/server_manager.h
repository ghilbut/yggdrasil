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
    void Create(const std::string& service_id, RequestHandle handler);
    void Destroy(const std::string& service_id);

    int HandleRequest(mg_connection* conn, mg_event ev);


private:
    void thread_main(void);


private:
    boost::mutex mutex_;
    boost::thread thread_;
    boost::atomic_bool alive_;
    boost::atomic_bool stop_;

    int next_port_;

    typedef std::map<std::string, Http::Server*> ServerMap;
    typedef std::map<int, RequestHandle> RequestHandleMap;
    ServerMap updates_;
    ServerMap servers_;
    RequestHandleMap request_handles_;
};

#endif  // SERVER_MANAGER_H_