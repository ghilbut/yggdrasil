#include "http_server.h"
#include <mongoose.h>


namespace Http {

Server::Server(void)
    : server_(0) {
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
        Destroy();
        return false;
    }

    port_ = port;
    return true;
}

void Server::Destroy(void) {
    mg_server* s = server_;
    server_ = 0;
    mg_destroy_server(&s);
}

int Server::Poll(int milliseconds) {
    if (server_ != 0) {
        return mg_poll_server(server_, milliseconds);
    }
    return 0;
}

int Server::port(void) const {
    return port_;
}

}  // namespace Http