#include "http_ui_object.h"

#include "boost_lib_fwd.h"
#include <mongoose/mongoose.h>
#include <json/json.h>


namespace Http {

UIObject::UIObject(const std::string& document_root, uint32_t port)
    : Object(document_root, port) {
    // nothing
}

void UIObject::BindCommonPathHandler(CommonPathHandler handle) {
    BOOST_ASSERT(common_path_handle_ == 0);
    common_path_handle_ = handle;
}

void UIObject::UnbindCommonPathHandler(void) {
    BOOST_ASSERT(common_path_handle_ != 0);
    common_path_handle_ = 0;
}

int UIObject::OnBeginRequest(mg_connection* conn) {
    // Returning non-zero tells mongoose that our function has replied to
    // the client, and mongoose should not send client any more data.
    const struct mg_request_info *ri = mg_get_request_info(conn);

    const char* uri = ri->uri;
    const char* method = ri->request_method;

    if (strcmp(method, "GET") == 0) {
        std::string filepath = (boost::filesystem::path(document_root()) / uri).string();
        if (boost::filesystem::exists(filepath)) {
            return 0;
        }
        if (common_path_handle_) {
            if (common_path_handle_(uri, filepath)) {
                mg_send_file(conn, filepath.c_str());
                return 1;
            }
        }
        // TODO(jh81.kim): handle pretty 404 page
        return 0;
    }

    if (strcmp(method, "POST") == 0) {
        // forward command over channel
        if (DoRequest(conn, method, uri)) {
            // TODO(ghilbut):
            // how can I response ?
        } else {
            // TODO(ghilbut):
            // make error response
            mg_printf(conn
                , "HTTP/1.1 %d Error (%s)\r\n\r\n%s"
                , 500 /* This the error code you want to send back*/
                , "Invalid Request."
                , "Request failed.");
        }
        return 1;
    }

    // TODO(ghilbut): use 4XX error
    mg_printf(conn
        , "HTTP/1.1 %d Error (%s)\r\n\r\n%s"
        , 500 /* This the error code you want to send back*/
        , "Invalid Request."
        , "Invalid Request.");

    return 1;
}

void UIObject::OnEndRequest(const struct mg_connection* conn, int reply_status_code) {
    // nothing
}

int UIObject::OnWebsocketConnect(const struct mg_connection* conn) {
    // nothing
    return 0;
}

void UIObject::OnWebsocketReady(struct mg_connection* conn) {
    websockets_.Register(conn);
}

int UIObject::OnWebsocketData(struct mg_connection* conn, int bits, char* data, size_t data_len) {

    const char opcode = static_cast<char>(bits & 0x0f);

    if (opcode == WEBSOCKET_OPCODE_CONTINUATION) {
        // nothing
        return 1;
    }

    if (opcode == WEBSOCKET_OPCODE_TEXT) {
        if (data_len > 0) {
            std::string text(data, data + data_len);
            if (text != "rabbit_hole_ping") {
                DoNotify(text);
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

void UIObject::FireEvent(const std::string& json) {
    SendWebSocketData(json);
}

void UIObject::SendWebSocketData(const std::string& data) {
    websockets_.SendText(data);
}

void UIObject::PingWebSockets(void) {
    websockets_.Ping();
}

static int HandleFile(mg_connection* conn
                      , const char* filepath) {
        if (boost::filesystem::is_regular_file(filepath)) {
            mg_send_file(conn, filepath);
            return 1;
        }
        return 0;
}

}  // namespace Http