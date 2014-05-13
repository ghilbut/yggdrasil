#include "http_websocket.h"
#include "http_message.h"
#include <mongoose.h>
#include <boost/bind.hpp>


namespace Http {

WebSocket::WebSocket(Environ* env, struct mg_connection* conn)
    : isolate_(env->isolate())
    , strand_(env->io_service())
    , conn_(conn) {
    // nothing
}

WebSocket::~WebSocket(void) {
    // nothing
}

void WebSocket::WeakCallback(const v8::WeakCallbackData<v8::Object, WebSocket>& data) {
    WebSocket* pThis = data.GetParameter();
    (pThis->self_).Reset();
    delete pThis;
}

void WebSocket::MakeWeak(v8::Isolate* isolate, v8::Local<v8::Object> self) {
    v8::HandleScope handle_scope(isolate);
    self_.Reset(isolate, self);
    self->SetAlignedPointerInInternalField(0, this);
    self_.MarkIndependent();
    self_.SetWeak(this, WeakCallback);
}

void WebSocket::ClearWeak(void) {
    self_.ClearWeak();
}

void WebSocket::DoSend(const Message& msg) const {
    strand_.post(boost::bind(&WebSocket::handle_send, this, msg));
}

void WebSocket::handle_send(const Message& msg) const {
    mg_websocket_write(conn_, 0x1, msg.data(), msg.data_len());
}

}  // namespace Http
