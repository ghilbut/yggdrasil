#include "http_ui_object.h"

#include "http_fwd.h"
#include "boost_lib_fwd.h"
#include <mongoose/mongoose.h>
#include <json/json.h>


namespace Http {

UIObject::UIObject(const std::string& document_root, uint32_t port)
    : Object(document_root, port) {
    // nothing
}

void UIObject::BindCommonPathHandler(CommonPathHablder handle) {
    BOOST_ASSERT(common_path_handle_ == 0);
    common_path_handle_ = handle;
}

void UIObject::UnbindCommonPathHandler(void) {
    BOOST_ASSERT(common_path_handle_ != 0);
    common_path_handle_ = 0;
}

int UIObject::OnRequest(mg_connection* conn) {

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
        BOOST_ASSERT(common_path_handle_ != 0);
        if (common_path_handle_ == 0) {
            // TODO(ghilbut): what can i do ? 
            return 0;
        }

        std::string filepath;
        if (common_path_handle_(uri, filepath)) {
            mg_send_file(conn, filepath.c_str());
        } else {
            // TODO(ghilbut): use 404 error
            mg_printf(conn
                     , "HTTP/1.1 %d Error (%s)\r\n\r\n%s"
                     , 500 /* This the error code you want to send back*/
                     , "Invalid Request."
                     , "Invalid Request.");
        }

        return 1;
    }

    // local static files
    if (strncmp(uri, kStaticKeyword, kStaticKeySize) == 0) {
        return 0;
    }

    // forward command over channel
    if (strncmp(uri, kRequestKeyword, kRequestKeySize) == 0) {
        if (DoRequest(conn, method, uri + kRequestKeySize)) {
            // TODO(ghilbut):
            // how can I response ?
        } else {
            // TODO(ghilbut):
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

    // TODO(ghilbut): use 4XX error
    mg_printf(conn
                , "HTTP/1.1 %d Error (%s)\r\n\r\n%s"
                , 500 /* This the error code you want to send back*/
                , "Invalid Request."
                , "Invalid Request.");

    return 1;
}

void UIObject::OnWebsocketText(const std::string& data) {
    DoNotify(data);
}

void UIObject::FireEvent(const std::string& json) {
    SendWebSocketData(json);
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