#include "http_websocket_manager.h"
#include <mongoose/mongoose.h>
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

void WebsocketManager::FireEvent(const std::string& text) {
    boost::mutex::scoped_lock lock(mutex_);
    std::vector<struct mg_connection*> errors;
    std::set<struct mg_connection*>::iterator itr = websockets_.begin();
    std::set<struct mg_connection*>::iterator end = websockets_.end();
    for (; itr != end; ++itr) {
        struct mg_connection* conn = *itr;
        const int written_bytes = mg_websocket_write(conn, WEBSOCKET_OPCODE_TEXT, &text[0], text.length());
        if (written_bytes < 0) {
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

void WebsocketManager::Ping(void) {
    boost::mutex::scoped_lock lock(mutex_);
    std::vector<struct mg_connection*> errors;
    std::set<struct mg_connection*>::iterator itr = websockets_.begin();
    std::set<struct mg_connection*>::iterator end = websockets_.end();
    for (; itr != end; ++itr) {
        struct mg_connection* conn = *itr;
        const int written_bytes = mg_websocket_write(conn, WEBSOCKET_OPCODE_PING, 0, 0);
        if (written_bytes == 0) {
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