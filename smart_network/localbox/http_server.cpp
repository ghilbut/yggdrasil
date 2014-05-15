#include "http_server.h"
#include "template_factory.h"
#include "frontend/http_message.h"
#include "frontend/http_request.h"
#include "frontend/http_request_template.h"
#include "frontend/http_response.h"
#include "frontend/http_response_impl.h"
#include "frontend/http_response_template.h"
#include "frontend/http_websocket_template.h"
#include <mongoose.h>
#include <ctime>
#include <cstdlib>



namespace Http {

Server::Server(Environ* env)
    : env_(env)
    , port_(env->port())
    , is_alive_(false)
    , is_stop_(false)
    , req_manager_(env, self_)
    , ws_manager_(env, self_) {
   // nothing
}

Server::~Server(void) {
    DoClose();
}

void Server::WeakCallback(const v8::WeakCallbackData<v8::Object, Server>& data) {
  Server* pThis = data.GetParameter();
  (pThis->self_).Reset();
  delete pThis;
}

void Server::MakeWeak(v8::Isolate* isolate, v8::Local<v8::Object> self) {
    v8::HandleScope handle_scope(isolate);
    self_.Reset(isolate, self);
    self->SetAlignedPointerInInternalField(0, this);
    self_.MarkIndependent();
    self_.SetWeak(this, WeakCallback);
}

void Server::ClearWeak(void) {
    self_.ClearWeak();
}

bool Server::DoListen(void) {
    boost::promise<bool> promise;

    void (boost::promise<bool>::*setter)(const bool&) = &boost::promise<bool>::set_value;
    boost::function<void(const bool&)> promise_setter = boost::bind(setter, &promise, _1);
    env_->Post(boost::bind(&Server::handle_listen, this, promise_setter));

    return promise.get_future().get();
}

void Server::DoClose(void) {
    if (!is_stop_) {
        env_->Post(boost::bind(&Server::handle_close, this));
    }
    if (thread_.joinable()) {
        thread_.join();
    }
}

void Server::DoNotify(const Message& msg) {
    ws_manager_.DoNotify(msg);
}

v8::Local<v8::Object> Server::request_trigger(v8::Isolate* isolate) const {
    return req_manager_.request_trigger(isolate);
}

void Server::set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger) {
    req_manager_.set_request_trigger(isolate, trigger);
}

v8::Local<v8::Object> Server::open_trigger(v8::Isolate* isolate) const {
    return ws_manager_.open_trigger(isolate);
}

void Server::set_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger) {
    ws_manager_.set_open_trigger(isolate, trigger);
}

void Server::handle_listen(boost::function<void(const bool&)> ret_setter) {
    if (is_alive_) {
        ret_setter(false);
        return;
    }

    is_alive_ = true;
    is_stop_ = false;

    if (thread_.joinable()) {
        thread_.join();
    }
    //thread_.swap(boost::thread(boost::bind(&Server::thread_main, this)));
    ret_setter(true);
}

void Server::handle_close() {
    is_stop_ = true;
}

int Server::request_handler(struct mg_connection *conn, enum mg_event ev) {

    if (ev == MG_REQUEST) {
        Server* s = static_cast<Server*>(conn->server_param);
        if (conn->is_websocket) {
            (s->ws_manager_).HandleMessage(conn);
        } else {
            Response res = (s->req_manager_).HandleRequest(conn);
            res.Send(conn);
        }
        return MG_TRUE;
    } else if (ev == MG_AUTH) {
        return MG_TRUE;
    } else {
        return MG_FALSE;
    }
}

void Server::thread_main(void) {

    mg_server* server = mg_create_server(this, &Server::request_handler);
    if (server == 0) {
        goto ERROR0;
    }

    char sport[10];
    sprintf(sport, "%d", port_);
    const char* error_msg = mg_set_option(server, "listening_port", sport);
    if (error_msg) {
        // TODO(ghilbut): delegate error
        goto ERROR1;
    }

    while (is_stop_ == false) {
        mg_poll_server(server, 100);
    }

ERROR1:
    mg_destroy_server(&server);
ERROR0:
    is_alive_ = false;
}

}  // namespace Http