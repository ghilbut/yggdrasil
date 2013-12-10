#include "mongoose_mock.h"
#include <mongoose/mongoose.h>


static int begin_request(mg_connection* conn) {
    struct mg_request_info* ri = mg_get_request_info(conn);
    MongooseMock* mock = static_cast<MongooseMock*>(ri->user_data);
    return mock->OnBeginRequest(conn);
}

static void end_request(const struct mg_connection* conn, int reply_status_code) {
    struct mg_connection* conn2 = const_cast<struct mg_connection*>(conn);
    struct mg_request_info* ri = mg_get_request_info(conn2);
    MongooseMock* mock = static_cast<MongooseMock*>(ri->user_data);
    return mock->OnEndRequest(conn, reply_status_code);
}

static int websocket_connect(const struct mg_connection* conn) {
    struct mg_connection* conn2 = const_cast<struct mg_connection*>(conn);
    const struct mg_request_info* info = mg_get_request_info(conn2);
    MongooseMock* mock = static_cast<MongooseMock*>(info->user_data);
    return mock->OnWebsocketConnect(conn);
}

static void websocket_ready(struct mg_connection* conn) {
    const struct mg_request_info* info = mg_get_request_info(conn);
    MongooseMock* mock = static_cast<MongooseMock*>(info->user_data);
    mock->OnWebsocketReady(conn);
}

static int websocket_data(struct mg_connection* conn, int bits, char* data, size_t data_len) {
    const struct mg_request_info* info = mg_get_request_info(conn);
    MongooseMock* mock = static_cast<MongooseMock*>(info->user_data);
    return mock->OnWebsocketData(conn, bits, data, data_len);
}

static const mg_callbacks kHttpCallbacks = {
    begin_request,     // int begin_request(mg_connection* conn);
    end_request,       // void end_request(const struct mg_connection *, int reply_status_code);
    0,                 // int  log_message(const struct mg_connection *, const char *message);
    0,                 // int  init_ssl(void *ssl_context);
    websocket_connect, // int  websocket_connect(const struct mg_connection *);
    websocket_ready,   // void websocket_ready(struct mg_connection *);
    websocket_data,    // int  websocket_data(struct mg_connection *);
    0,                 // const char* open_file(const struct mg_connection *, const char *path, size_t *data_len);
    0,                 // void init_lua(struct mg_connection *, void *lua_context);
    0                  // void upload(struct mg_connection *, const char *file_name);
};


MongooseMock::MongooseMock(const char* document_root, uint32_t port) {
    char sport[16];
    sprintf(sport, "%u", port);
    const char *options[] = {
        "document_root", document_root,
        "enable_directory_listing", "no",
        "listening_ports", sport, 
        NULL};
    ctx_ = mg_start(&kHttpCallbacks, this, options);
}

MongooseMock::~MongooseMock(void) {
    mg_stop(ctx_);
}