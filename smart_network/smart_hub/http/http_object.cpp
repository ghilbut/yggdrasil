#include "http_object.h"

#include "http_fwd.h"
#include <mongoose/mongoose.h>
#include <json/json.h>
#include <boost/assert.hpp>
#include <boost/filesystem.hpp>


HttpObject::HttpObject(const std::string& document_root, uint32_t port) 
    : httpd_(0)
    , document_(document_root)
    , port_(port) {
}

HttpObject::~HttpObject(void) {
	
}

int HttpObject::OnBeginRequest(mg_connection* conn) {
    // Returning non-zero tells mongoose that our function has replied to
    // the client, and mongoose should not send client any more data.

    const struct mg_request_info *ri = mg_get_request_info(conn);

    const char* uri = ri->uri;
    const char* method = ri->request_method;

    // basic static files
    if (strcmp(uri, "/") == 0 
        || strcmp(uri, "/favicon.ico") == 0 
        || strcmp(uri, "/active.png") == 0 
        || strcmp(uri, "/disabled.png") == 0) {
        return 0;
    }

    // common static files
    if (strncmp(uri, kCommonKeyword, kCommonKeySize) == 0) {
        if (common_handler_) {
            return common_handler_(conn, method, uri);
        }

        mg_printf(conn
                 , "HTTP/1.1 %d Error (%s)\r\n\r\n%s"
                 , 500 /* This the error code you want to send back*/
                 , "Invalid Request."
                 , "Invalid Request.");

        return 1;
    }

    // local static files
    if (strncmp(uri, kStaticKeyword, kStaticKeySize) == 0) {
        return 0;
    }

    // forward command over channel
    if (strncmp(uri, kCommandKeyword, kCommandKeySize) == 0) {
        if (DoRequest(conn, method, uri + kCommandKeySize)) {
            // TODO(jh81.kim):
            // how can I response ?
        } else {
            // TODO(jh81.kim):
            // make error response
        }
        return 1;
    }

    // TODO(jsh81.kim): execute internal script
    // using V8 javascript engine [RESERVED]
    if (strncmp(uri, kScriptKeyword, kScriptKeySize) == 0) {
        return DoExecute(conn, method, uri);
    }



    char test[260];
    if (sscanf(ri->uri, "/test/%s\0", test) == 1) {

        static char content[100];
        memset(content, 0, sizeof(content));
        int content_length = sprintf(content, "Hello from mongoose! Remote port: %d  >> %s", ri->remote_port, ri->uri);

        // Send HTTP reply to the client
        mg_printf(conn,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %d\r\n"        // Always set Content-Length
            "\r\n"
            "%s",
            content_length, content);
        return 1;
    }

    



    return 1;
}

void HttpObject::OnEndRequest(const struct mg_connection* conn, int reply_status_code) {
    // nothing
}

static void InsertWebsocket(std::set<struct mg_connection*>& sockets, mg_connection* conn, boost::mutex& mutex) {
    boost::mutex::scoped_lock lock(mutex);
    BOOST_ASSERT(sockets.find(conn) == sockets.end());
    sockets.insert(conn);
}

static void RemoveWebsocket(std::set<struct mg_connection*>& sockets, mg_connection* conn, boost::mutex& mutex) {
    boost::mutex::scoped_lock lock(mutex);
    BOOST_ASSERT(sockets.find(conn) != sockets.end());
    sockets.erase(conn);
}

int HttpObject::OnWebsocketConnect(const struct mg_connection* conn) {
    // nothing
    return 0;
}

void HttpObject::OnWebsocketReady(struct mg_connection* conn) {
    ::InsertWebsocket(websockets_, conn, mutex_);
}

int  HttpObject::OnWebsocketData(struct mg_connection* conn, int bits, char* data, size_t data_len) {
    BOOST_ASSERT(websockets_.find(conn) != websockets_.end());

    const char opcode = static_cast<char>(bits & 0x0f);

    if (opcode == WEBSOCKET_OPCODE_CONTINUATION) {
        // nothing
        return 1;
    }

    if (opcode == WEBSOCKET_OPCODE_TEXT) {
        std::string text(data, data + data_len);
        mg_websocket_write(conn, WEBSOCKET_OPCODE_TEXT, &text[0], text.length());
        return 1;
    }

    if (opcode == WEBSOCKET_OPCODE_BINARY) {
        // nothing
        return 1;
    }

    if (opcode == WEBSOCKET_OPCODE_CONNECTION_CLOSE) {
        ::RemoveWebsocket(websockets_, conn, mutex_);
        return 0;
    }

    if (opcode == WEBSOCKET_OPCODE_PING) {
        return mg_websocket_write(conn, WEBSOCKET_OPCODE_PONG, 0, 0);
    }

    if (opcode == WEBSOCKET_OPCODE_PONG) {
        // TODO(jh81.kim):
        // should I check the pair 
        // between PING request and this PONG response ?
        return 1;
    }

    return 1;
}


void HttpObject::BindCommonHandler(RequestHandler handler) {
    BOOST_ASSERT(handler);
    common_handler_ = handler;
}

void HttpObject::UnbindCommonHandler(void) {
    common_handler_ = 0;
}

void HttpObject::PingWebSockets(void) {
    boost::mutex::scoped_lock lock(mutex_);
    std::set<struct mg_connection*>::iterator itr = websockets_.begin();
    std::set<struct mg_connection*>::iterator end = websockets_.end();
    for (; itr != end; ++itr) {
        struct mg_connection* conn = *itr;
        const int written_bytes = mg_websocket_write(conn, WEBSOCKET_OPCODE_PING, 0, 0);
        if (written_bytes == 0) {
            ::RemoveWebsocket(websockets_, conn, mutex_);
        }
    }
}

void HttpObject::FireEvent(const std::string& json) {
    boost::mutex::scoped_lock lock(mutex_);
    std::set<struct mg_connection*>::iterator itr = websockets_.begin();
    std::set<struct mg_connection*>::iterator end = websockets_.end();
    for (; itr != end; ++itr) {
        const char* data = &json[0];
        const size_t data_len = json.length();
        struct mg_connection* conn = *itr;
        const int written_bytes = mg_websocket_write(conn, WEBSOCKET_OPCODE_TEXT, data, data_len);
        if (written_bytes < data_len) {
            ::RemoveWebsocket(websockets_, conn, mutex_);
        }
    }
}

struct mg_context* HttpObject::context(void) const {
    return httpd_;
}

void HttpObject::set_context(struct mg_context* ctx) {
    httpd_ = ctx;
}

const char* HttpObject::document(void) const {
    return document_.c_str();
}

uint32_t HttpObject::port(void) const {
    return port_;
}

static int HandleFile(mg_connection* conn
                      , const char* filepath) {
        if (boost::filesystem::is_regular_file(filepath)) {
            mg_send_file(conn, filepath);
            return 1;
        }
        return 0;
}