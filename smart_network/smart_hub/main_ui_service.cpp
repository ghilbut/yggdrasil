#include "main_ui_service.h"
#include "main_ui_handler.h"
#include "codebase/boost_lib_fwd.h"
#include <mongoose/mongoose.h>


MainUIService::MainUIService(const std::string& document_root, uint32_t port)
    : Http::UIObject(document_root, port)
    , ui_handler_(0) {
}

void MainUIService::BindUIHandler(MainUIHandler* delegator) {
    BOOST_ASSERT(ui_handler_ == 0);
    ui_handler_ = delegator;
}

void MainUIService::UnbindUIHandler(void) {
    BOOST_ASSERT(ui_handler_ != 0);
    ui_handler_ = 0;
}

bool MainUIService::FireRequest(mg_connection* conn, const char* method, const char* query) {

    if (strcmp(query, "/get/device/list") == 0) {

        std::string json;
        ui_handler_->handle_get_device_list(json);

        const char* response = ""
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"        // Always set Content-Length
            "\r\n"
            "%s";
        const int result = mg_printf(conn, response, json.length(), json.c_str());
        if (result == 0) {
            // TODO(ghilbut):
            // connection has been closed
            return false;
        }
        if (result < 0) {
            // TODO(ghilbut):
            // error
            return false;
        }
        return true;
    }

    return false;
}

bool MainUIService::FireNotify(const std::string& text) {
    return false;
}