#include "http_server.h"
#include "http_fwd.h"
#include "http_object.h"
#include <mongoose/mongoose.h>
#include <set>
#include <map>


int begin_request(mg_connection* conn) {
    struct mg_request_info* ri = mg_get_request_info(conn);
    HttpObject* http = static_cast<HttpObject*>(ri->user_data);
    return http->OnBeginRequest(conn);
}

void end_request(const struct mg_connection* conn, int reply_status_code) {
    struct mg_connection* conn2 = const_cast<struct mg_connection*>(conn);
    struct mg_request_info* ri = mg_get_request_info(conn2);
    HttpObject* http = static_cast<HttpObject*>(ri->user_data);
    return http->OnEndRequest(conn, reply_status_code);
}

int websocket_connect(const struct mg_connection* conn) {
    struct mg_connection* conn2 = const_cast<struct mg_connection*>(conn);
    const struct mg_request_info* info = mg_get_request_info(conn2);
    HttpObject* http = static_cast<HttpObject*>(info->user_data);
    return http->OnWebsocketConnect(conn);
}

void websocket_ready(struct mg_connection* conn) {
    const struct mg_request_info* info = mg_get_request_info(conn);
    HttpObject* http = static_cast<HttpObject*>(info->user_data);
    http->OnWebsocketReady(conn);
}

int  websocket_data(struct mg_connection* conn, int bits, char* data, size_t data_len) {
    const struct mg_request_info* info = mg_get_request_info(conn);
    HttpObject* http = static_cast<HttpObject*>(info->user_data);
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


static mg_context* HttpStart(HttpObject* http) {
    BOOST_ASSERT(http);

    printf("[HTTP][Start] port: %d, root: %s.\n", http->port(), http->document());

    char sport[16];
    sprintf(sport, "%d", http->port());
    const char *options[] = {
        "document_root", http->document(),
        "enable_directory_listing", "no",
        "listening_ports", sport, 
        NULL};
    mg_context* ctx = mg_start(&kHttpCallbacks, http, options);
    if (ctx) {
        http->set_context(ctx);
    }

    printf("[HTTP][Start] mg_start %s.\n", ctx ? "succeeded" : "failed");
    return ctx;
}

static void HttpStop(HttpObject* http) {
    BOOST_ASSERT(http);

    mg_context* ctx = http->context();
    if (ctx) {
        mg_stop(ctx);
        http->set_context(0);
    }
}


HttpServer::HttpServer(const std::string& common_root)
	: common_(common_root), websocket_(http_map_) {
}

HttpServer::~HttpServer(void) {
}

bool HttpServer::Insert(HttpObject* http) {

    if (!boost::filesystem::is_directory(common_)
        || !boost::filesystem::is_directory(http->document())) {
        return false;
    }

    mg_context* ctx = HttpStart(http);
    if (!ctx) {
        return false;
    } 

    BOOST_ASSERT(http_map_.find(ctx) == http_map_.end());
    http->BindCommonHandler(boost::bind(&HttpServer::DoHandleCommon, this, _1, _2, _3));
    http_map_[ctx] = http;
    return true;
}

void HttpServer::Remove(HttpObject* http) {
    mg_context* ctx = http->context();
    HttpStop(http);
    http->UnbindCommonHandler();
    http_map_.erase(ctx);
}


static int HandleFile(mg_connection* conn
                      , const char* filepath) {
    if (boost::filesystem::is_regular_file(filepath)) {
        mg_send_file(conn, filepath);
        return 1;
    }
    return 0;
}

int HttpServer::DoHandleCommon(mg_connection* conn
                                , const char* method
                                , const char* uri) const {
    boost::filesystem::path filepath(common_);
    filepath /= (uri + kCommonKeySize);
    return HandleFile(conn, filepath.string().c_str());
}
