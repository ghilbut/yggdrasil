#include "http_websocket_manager.h"
#include <mongoose.h>
#include <vector>


namespace Http {

WebsocketManager::WebsocketManager(void) {
    // nothing
}

WebsocketManager::~WebsocketManager(void) {
    // nothing
}

void WebsocketManager::Register(struct mg_connection* conn) {
    boost::mutex::scoped_lock lock(mutex_);
    websockets_.insert(conn);
}

void WebsocketManager::Unregister(struct mg_connection* conn) {
    boost::mutex::scoped_lock lock(mutex_);
    websockets_.erase(conn);
}

void WebsocketManager::SendText(const std::string& text) {
    SendData(WEBSOCKET_OPCODE_TEXT, &text[0], text.length());
}

void WebsocketManager::Ping(void) {
    SendData(WEBSOCKET_OPCODE_PING, 0, 0);
}

void WebsocketManager::SendData(int opcode, const char* data, size_t data_len) {
    boost::mutex::scoped_lock lock(mutex_);
    std::vector<struct mg_connection*> errors;
    std::set<struct mg_connection*>::iterator itr = websockets_.begin();
    std::set<struct mg_connection*>::iterator end = websockets_.end();
    for (; itr != end; ++itr) {
        struct mg_connection* conn = *itr;
        const int written_bytes = mg_websocket_write(conn, opcode, data, data_len);
        if (written_bytes < 1) {
            errors.push_back(conn);
        }
    }

    if (!errors.empty()) {
        const size_t size = errors.size();
        for (int index = 0; index < size; ++index) {
            websockets_.erase(errors[index]);
        }
    }
}

}  // namespace http