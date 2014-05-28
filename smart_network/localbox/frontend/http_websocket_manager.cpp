#include "http_websocket_manager.h"

#include "http_message.h"
#include "http_websocket.h"
#include "basebox/device.h"
#include "js/frontend/http_websocket_template.h"
#include <mongoose.h>
#include <boost/bind.hpp>


namespace Http {

WebSocketManager::WebSocketManager(const DeviceRef& device, v8::Persistent<v8::Object>& service, v8::Persistent<v8::Object>& caller) 
    : device_(device)
    , service_(service)
    , caller_(caller) {
    // nothing
}

WebSocketManager::~WebSocketManager(void) {
    // nothing
}

void WebSocketManager::HandleMessage(mg_connection* conn) {
    v8::Isolate* isolate = device_->isolate(); 
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    WebSocketMap::iterator itr = websockets_.find(conn);
    if (itr == websockets_.end()) {
        device_->Post(boost::bind(&WebSocketManager::event_open, this, conn));
    }

    switch (conn->wsbits & 0x0f) {
    case 0x00:
        // denotes a continuation frame
        break;
    case 0x01:
        {
            const size_t data_len = conn->content_len;
            char* data = new char[data_len];
            memcpy(data, conn->content, data_len);
            const Message msg(data, data_len);
            device_->Post(boost::bind(&WebSocketManager::event_message, this, conn, msg));
        }
        break;
    case 0x02:
        // denotes a binary frame
        break;
    case 0x08:
        // denotes a connection close
        {
            device_->Post(boost::bind(&WebSocketManager::event_closed, this, conn));
        }
        break;
    case 0x09:
        // denotes a ping
        break;
    case 0x0A:
        // denotes a pong
        break;
    default:
        // %x3-7 are reserved for further non-control frames
        // %xB-F are reserved for further control frames
        break;
    }
}

void WebSocketManager::DoNotify(const Message& msg) {
    device_->Post(boost::bind(&WebSocketManager::trigger_notify, this, msg));
}

v8::Local<v8::Object> WebSocketManager::open_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_open_);
}
void WebSocketManager::set_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    on_open_.Reset(isolate, trigger);
}

v8::Local<v8::Object> WebSocketManager::message_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_message_);
}
void WebSocketManager::set_message_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    on_message_.Reset(isolate, trigger);
}

v8::Local<v8::Object> WebSocketManager::closed_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Object>::New(isolate, on_closed_);
}
void WebSocketManager::set_closed_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger) {
    on_closed_.Reset(isolate, trigger);
}

void WebSocketManager::event_open(mg_connection* conn) {
    v8::Isolate* isolate = device_->isolate(); 
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    WebSocket websocket(device_, service_, conn);
    websockets_[conn] = websocket;

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::New(isolate, on_open_);
    if (!obj.IsEmpty() && obj->IsFunction() && obj->IsCallable()) {
        v8::Local<v8::Value> params[1] = { websocket.self(isolate) };
        obj->CallAsFunction(v8::Local<v8::Object>::New(isolate, caller_), 1, params);
    }
}

void WebSocketManager::event_message(mg_connection* conn, const Message& msg) {
    WebSocketMap::iterator itr = websockets_.find(conn);
    if (itr != websockets_.end()) {
       (itr->second).FireMessage(msg);
    }
}

void WebSocketManager::event_closed(mg_connection* conn) {
    WebSocketMap::iterator itr = websockets_.find(conn);
    if (itr != websockets_.end()) {
        (itr->second).FireClosed();
        websockets_.erase(itr);
    }
}

void WebSocketManager::trigger_notify(const Message& msg) {
    if (!websockets_.empty()) {
        WebSocketMap::const_iterator itr = websockets_.begin();
        WebSocketMap::const_iterator end = websockets_.end();
        for (; itr != end; ++itr) {
            mg_connection* conn = itr->first;
            mg_websocket_write(conn, 0x01, msg.data(), msg.data_len());
        }
    }
}

}  // namespace Http
