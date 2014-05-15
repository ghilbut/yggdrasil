#include "http_server.h"
#include <mongoose.h>


namespace Http {

Server::Server(void)
    : server_(0)
    , paused_(true) {
   // nothing
}

Server::~Server(void) {
    Destroy();
}

bool Server::Create(void* server_param, mg_handler_t handler, int port) {

    server_ = mg_create_server(server_param, handler);
    if (server_ == 0) {
        return false;
    }

    char sport[10];
    sprintf(sport, "%d", port);
    const char* error_msg = mg_set_option(server_, "listening_port", sport);
    if (error_msg) {
        mg_destroy_server(&server_);
        return false;
    }

    paused_ = false;
    return true;
}

void Server::Destroy(void) {
    paused_ = true;
    mg_destroy_server(&server_);
}

void Server::Pause(void) {
    paused_ = true;
}

void Server::Resume(void) {
    paused_ = false;
}

int Server::Poll(int milliseconds) {
    if (!paused_) {
        return mg_poll_server(server_, milliseconds);
    }
    return 0;
}

}  // namespace Http