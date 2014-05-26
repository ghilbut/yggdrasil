#include "server_manager.h"

#include <mongoose.h>
#include <boost/bind.hpp>


static int http_request_handler(struct mg_connection* conn, enum mg_event ev) {
    ServerManager* s = static_cast<ServerManager*>(conn->server_param);
    return s->HandleRequest(conn, ev);
};


ServerManager::ServerManager(void)
    : thread_(boost::bind(&ServerManager::thread_main, this))
    , alive_(false)
    , stop_(false)
    , next_port_(81) {

}

ServerManager::~ServerManager(void) {

}

void ServerManager::Create(const std::string& service_id, RequestHandle handler) {

    const int port = next_port_++;
    mutex_.lock();
    Http::Server* s = new Http::Server();
    s->Create(this, http_request_handler, port);
    updates_[service_id] = s;
    request_handles_[port] = handler;
    mutex_.unlock();
}

void ServerManager::Destroy(const std::string& service_id) {
    ServerMap::const_iterator itr = servers_.find(service_id);
    if (itr != servers_.end()) {
        // TODO(ghilbut): reg to updates
        mutex_.lock();
        mutex_.unlock();
    }
}

int ServerManager::HandleRequest(mg_connection* conn, mg_event ev) {
    const int port = conn->local_port;
    RequestHandleMap::const_iterator itr = request_handles_.find(port);
    if (itr == request_handles_.end()) {
        return MG_FALSE;
    }
    return (itr->second)(conn, ev);
}


void ServerManager::thread_main(void) {
    alive_ = true;
    while (!stop_) {

        mutex_.lock();
        if (!updates_.empty()) {
            servers_.insert(updates_.begin(), updates_.end());
        }
        mutex_.unlock();

        ServerMap::const_iterator itr = servers_.begin();
        ServerMap::const_iterator end = servers_.end();
        for (; itr != end; ++itr) {
            (itr->second)->Poll(10);
        }
    }
    alive_ = false;
}
