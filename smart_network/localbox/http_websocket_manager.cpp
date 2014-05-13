#include "http_websocket_manager.h"
#include "http_websocket.h"
#include "http_websocket_template.h"
#include "http_message.h"

#include "environ.h"
#include "http_message.h"
#include <mongoose.h>
#include <boost/bind.hpp>


namespace Http {

WebSocketManager::WebSocketManager(Environ* env) 
    : isolate_(env->isolate())
    , strand_(env->io_service()) {
    // nothing
}

WebSocketManager::~WebSocketManager(void) {
}

void WebSocketManager::HandleMessage(mg_connection* conn) {
    v8::Isolate* isolate = isolate_; 
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    WebSocketMap::iterator itr = websockets_.find(conn);
    if (itr == websockets_.end()) {
        strand_.post(boost::bind(&WebSocketManager::event_open, this, conn));
    }

    switch (conn->wsbits & 0x0f) {
    case 0x00:
        // denotes a continuation frame
        break;
    case 0x01:
        {
            const size_t data_len = conn->content_len;
            const char* data = new char[data_len];
            const Message msg(data, data_len);
            strand_.post(boost::bind(&WebSocketManager::event_message, this, conn, msg));
        }
        break;
    case 0x02:
        // denotes a binary frame
        break;
    case 0x08:
        // denotes a connection close
        {
            strand_.post(boost::bind(&WebSocketManager::event_closed, this, conn));
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
    strand_.post(boost::bind(&WebSocketManager::trigger_notify, this, msg));
}

void WebSocketManager::set_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Function>& trigger) {
    on_open_.Reset(isolate_, trigger);
}

v8::Local<v8::Function> WebSocketManager::message_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Function>::New(isolate, on_message_);
}

void WebSocketManager::set_message_trigger(v8::Isolate* isolate, v8::Handle<v8::Function>& trigger) {
    on_message_.Reset(isolate_, trigger);
}

void WebSocketManager::set_closed_trigger(v8::Isolate* isolate, v8::Handle<v8::Function>& trigger) {
    on_closed_.Reset(isolate_, trigger);
}

void WebSocketManager::event_open(mg_connection* conn) {
    v8::Isolate* isolate = isolate_; 
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::Object> websocket = WebSocketTemplate::NewInstance(isolate, conn);
    websockets_[conn].Reset(isolate, websocket);

    // TODO(ghilbut): fire event to script
}

void WebSocketManager::event_message(mg_connection* conn, const Message& msg) {
    v8::Isolate* isolate = isolate_; 
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    
    WebSocketMap::iterator itr = websockets_.find(conn);
    if (itr == websockets_.end()) {
       return;
    }

    if (on_message_.IsEmpty()) {
        return;
    }

    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate_, on_message_);
    if (!func->IsCallable()) {
        return;
    }

    v8::Handle<v8::Value> params[2];
    params[0] = v8::Local<v8::Object>::New(isolate, itr->second);
    params[1] = v8::String::NewFromUtf8(isolate_, conn->content, v8::String::kNormalString, conn->content_len);
    v8::Local<v8::Object> object = v8::Local<v8::Object>::New(isolate_, caller_);
    //func->Call(object, 2, params);
    func->Call(v8::Null(isolate), 2, params);
}

void WebSocketManager::event_closed(mg_connection* conn) {
    v8::Isolate* isolate = isolate_; 
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    WebSocketMap::iterator itr = websockets_.find(conn);
    if (itr != websockets_.end()) {
        websockets_.erase(conn);
    }

    // TODO(ghilbut): fire event to script
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
