#include "http_object.h"

#include "http_fwd.h"
#include "boost_lib_fwd.h"
#include <mongoose/mongoose.h>
#include <json/json.h>


static int begin_request(mg_connection* conn) {
    struct mg_request_info* ri = mg_get_request_info(conn);
    Http::Object* http = static_cast<Http::Object*>(ri->user_data);
    return http->OnBeginRequest(conn);
}

static void end_request(const struct mg_connection* conn, int reply_status_code) {
    struct mg_connection* conn2 = const_cast<struct mg_connection*>(conn);
    struct mg_request_info* ri = mg_get_request_info(conn2);
    Http::Object* http = static_cast<Http::Object*>(ri->user_data);
    return http->OnEndRequest(conn, reply_status_code);
}

static int websocket_connect(const struct mg_connection* conn) {
    struct mg_connection* conn2 = const_cast<struct mg_connection*>(conn);
    const struct mg_request_info* info = mg_get_request_info(conn2);
    Http::Object* http = static_cast<Http::Object*>(info->user_data);
    return http->OnWebsocketConnect(conn);
}

static void websocket_ready(struct mg_connection* conn) {
    const struct mg_request_info* info = mg_get_request_info(conn);
    Http::Object* http = static_cast<Http::Object*>(info->user_data);
    http->OnWebsocketReady(conn);
}

static int websocket_data(struct mg_connection* conn, int bits, char* data, size_t data_len) {
    const struct mg_request_info* info = mg_get_request_info(conn);
    Http::Object* http = static_cast<Http::Object*>(info->user_data);
    return http->OnWebsocketData(conn, bits, data, data_len);
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


namespace Http {

Object::Object(const std::string& document_root, uint32_t port)
    : ctx_(0), document_root_(document_root), port_(port) {
    // nothing
}

bool Object::Start(void) {
    BOOST_ASSERT(ctx_ == 0);
    if (ctx_ != 0) {
        char text[1024];
        sprintf(text, "[ERROR][Http::Object] already started.\n"
                      "       PORT: %u, DOCUMENT ROOT: %s\n", port_, document_root_.c_str());
        printf(text);
        return false;
    }

    printf("[HTTP][Start] port: %d, root: %s.\n", port_, document_root_.c_str());

    char sport[16];
    sprintf(sport, "%u", port_);
    const char *options[] = {
        "document_root", document_root_.c_str(),
        "enable_directory_listing", "no",
        "listening_ports", sport, 
        NULL};
    ctx_ = mg_start(&kHttpCallbacks, this, options);
  
    printf("[HTTP][Start] mg_start %s.\n", ctx_ ? "succeeded" : "failed");
    return (ctx_ != 0);
}

void Object::Stop() {
    BOOST_ASSERT(ctx_ != 0);
    if (ctx_) {
        mg_stop(ctx_);
        ctx_ = 0;
    }
}

uint32_t Object::port(void) const {
    return port_;
}

}  // namespace Http