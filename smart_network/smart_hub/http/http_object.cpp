#include "http_object.h"

#include "http_fwd.h"
#include "codebase/boost_lib_fwd.h"
#include <mongoose/mongoose.h>
#include <json/json.h>


int begin_request(mg_connection* conn) {
    struct mg_request_info* ri = mg_get_request_info(conn);
    Http::Object* http = static_cast<Http::Object*>(ri->user_data);
    return http->OnBeginRequest(conn);
}

void end_request(const struct mg_connection* conn, int reply_status_code) {
    struct mg_connection* conn2 = const_cast<struct mg_connection*>(conn);
    struct mg_request_info* ri = mg_get_request_info(conn2);
    Http::Object* http = static_cast<Http::Object*>(ri->user_data);
    return http->OnEndRequest(conn, reply_status_code);
}

int websocket_connect(const struct mg_connection* conn) {
    struct mg_connection* conn2 = const_cast<struct mg_connection*>(conn);
    const struct mg_request_info* info = mg_get_request_info(conn2);
    Http::Object* http = static_cast<Http::Object*>(info->user_data);
    return http->OnWebsocketConnect(conn);
}

void websocket_ready(struct mg_connection* conn) {
    const struct mg_request_info* info = mg_get_request_info(conn);
    Http::Object* http = static_cast<Http::Object*>(info->user_data);
    http->OnWebsocketReady(conn);
}

int  websocket_data(struct mg_connection* conn, int bits, char* data, size_t data_len) {
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

int Object::OnBeginRequest(mg_connection* conn) {
    // Returning non-zero tells mongoose that our function has replied to
    // the client, and mongoose should not send client any more data.
    return OnRequest(conn);
}

void Object::OnEndRequest(const struct mg_connection* conn, int reply_status_code) {
    // nothing
}

int Object::OnWebsocketConnect(const struct mg_connection* conn) {
    // nothing
    return 0;
}

void Object::OnWebsocketReady(struct mg_connection* conn) {
    websockets_.Register(conn);
}

int Object::OnWebsocketData(struct mg_connection* conn, int bits, char* data, size_t data_len) {

    const char opcode = static_cast<char>(bits & 0x0f);

    if (opcode == WEBSOCKET_OPCODE_CONTINUATION) {
        // nothing
        return 1;
    }

    if (opcode == WEBSOCKET_OPCODE_TEXT) {
        if (data_len > 0) {
            std::string text(data, data + data_len);
            if (text != "rabbit_hole_ping") {
                OnWebsocketText(text);
            }
        }
        return 1;
    }

    if (opcode == WEBSOCKET_OPCODE_BINARY) {
        // nothing
        return 1;
    }

    if (opcode == WEBSOCKET_OPCODE_CONNECTION_CLOSE) {
        websockets_.Unregister(conn);
        return 0;
    }

    if (opcode == WEBSOCKET_OPCODE_PING) {
        return mg_websocket_write(conn, WEBSOCKET_OPCODE_PONG, data, data_len);
    }

    if (opcode == WEBSOCKET_OPCODE_PONG) {
        // TODO(ghilbut):
        // should I check the pair 
        // between PING request and this PONG response ?
        return 1;
    }

    return 1;
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

void Object::SendWebSocketData(const std::string& data) {
    websockets_.SendText(data);
}

void Object::PingWebSockets(void) {
    websockets_.Ping();
}

uint32_t Object::port(void) const {
    return port_;
}

}  // namespace Http