#include "http_server_manager.h"




HttpServerManager::HttpServerManager(boost::asio::io_service& io_service, mg_handler_t handler, void* opaque)
    : thread_(boost::bind(&HttpServerManager::thread_main, this)) {

}

HttpServerManager::~HttpServerManager(void) {

}



void HttpServerManager::thread_main(void) {

    mg_server* server = mg_create_server(opaque_, request_handler_);
    mg_server* server1 = mg_create_server(opaque_, request_handler_);
    mg_server* server2 = mg_create_server(opaque_, request_handler_);
    if (server == 0) {
        goto ERROR0;
    }

    const char* error_msg = mg_set_option(server, "listening_port", "81");
    error_msg = mg_set_option(server1, "listening_port", "82");
    error_msg = mg_set_option(server2, "listening_port", "83");
    if (error_msg) {
        // TODO(ghilbut): delegate error
        goto ERROR1;
    }

    while (1) {
        mg_poll_server(server, 10);
        mg_poll_server(server1, 10);
        mg_poll_server(server2, 10);
    }

ERROR1:
    mg_destroy_server(&server2);
    mg_destroy_server(&server1);
    mg_destroy_server(&server);
ERROR0:
    return;
}