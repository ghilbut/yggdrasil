#include "http_server.h"
#include "http_request.h"
#include "http_request_template.h"
#include "http_response.h"
#include "http_response_template.h"
#include "http_message.h"
#include "template_factory.h"
#include <mongoose.h>
#include <ctime>
#include <cstdlib>



namespace Http {

Server::Server(Environ* env)
    : env_(env)
    , isolate_(env->isolate())
    , strand_(env->io_service())
    , port_(env->port())
    , is_alive_(false)
    , is_stop_(false) {
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
    strand_.post(boost::bind(&Server::handle_listen, this, promise_setter));

    return promise.get_future().get();
}

void Server::DoClose(void) {
    if (!is_stop_) {
        strand_.post(boost::bind(&Server::handle_close, this));
    }
    if (thread_.joinable()) {
        thread_.join();
    }
}

void Server::DoSend(const Message& msg) {
    strand_.post(boost::bind(&Server::handle_send, this, msg));
}

void Server::DoNotify(const Message& msg) {
    strand_.post(boost::bind(&Server::handle_notify, this, msg));
}

v8::Local<v8::Function> Server::request_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Function>::New(isolate, on_request_);
}

void Server::set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Function> trigger) {
    on_request_.Reset(isolate, trigger);
}

v8::Local<v8::Function> Server::message_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Function>::New(isolate, on_message_);
}

void Server::set_message_trigger(v8::Isolate* isolate, v8::Handle<v8::Function> trigger) {
    on_message_.Reset(isolate, trigger);
}

v8::Local<v8::Function> Server::error_trigger(v8::Isolate* isolate) const {
    return v8::Local<v8::Function>::New(isolate, on_error_);
}

void Server::set_error_trigger(v8::Isolate* isolate, v8::Handle<v8::Function> trigger) {
    on_error_.Reset(isolate, trigger);
}

ResponsePtr Server::FireRequest(struct mg_connection *conn) {
    boost::promise<ResponsePtr> promise;

    void (boost::promise<ResponsePtr>::*setter)(const ResponsePtr&) = &boost::promise<ResponsePtr>::set_value;
    boost::function<void(const ResponsePtr&)> promise_setter = boost::bind(setter, &promise, _1);
    strand_.post(boost::bind(&Server::handle_request, this, conn, promise_setter));

    return promise.get_future().get();
}

void Server::FireMessage(struct mg_connection *conn) {
    strand_.post(boost::bind(&Server::handle_message, this, conn));
}

void Server::FireError(void) {
    strand_.post(boost::bind(&Server::handle_error, this));
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
    thread_.swap(boost::thread(boost::bind(&Server::thread_main, this)));
    ret_setter(true);
}

void Server::handle_close() {
    is_stop_ = true;
}

void Server::handle_send(const Message msg) {
    // TODO(ghilbut): send to single websocket.
    handle_notify(msg);
}

void Server::handle_notify(const Message msg) {
    std::map<mg_connection*, mg_connection*>::const_iterator itr = websockets_.begin();
    std::map<mg_connection*, mg_connection*>::const_iterator end = websockets_.end();
    for (; itr != end; ++itr) {
        mg_websocket_write(itr->first, 0x1, msg.data(), msg.data_len());
    }
}

void Server::handle_request(struct mg_connection *conn, boost::function<void(const ResponsePtr&)> ret_setter) {
    if (on_request_.IsEmpty()) {
        // TODO(ghilbut): error handling
        mg_send_data(conn, 0, 0);
        return;
    }

    v8::Isolate* isolate = isolate_; 
    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);

    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate_, on_request_);
    if (!func->IsCallable()) {
        // TODO(ghilbut): error handling
        mg_send_data(conn, 0, 0);
        return;
    }

    v8::Handle<v8::Value> params[1];
    Request* req = new Request(conn);
    params[0] = RequestTemplate::NewInstance(isolate_, req);

    v8::Local<v8::Object> object = v8::Local<v8::Object>::New(isolate_, self_);
    v8::Local<v8::Value> retval = func->Call(object, 1, params);

    if (retval->IsObject()) {
        v8::Local<v8::Object> obj = retval->ToObject();

        TemplateFactory& tf = env_->template_factory();
        v8::Local<v8::FunctionTemplate> rt = tf.ResponseTemplate(isolate);
        if (obj->GetConstructor() == rt->GetFunction()) {
            Response* res = static_cast<Response*>(obj->GetAlignedPointerFromInternalField(0));
            ret_setter(ResponsePtr(res));
            //res->ClearWeak();
            return;
        }
    }

    v8::Local<v8::String> str = retval->ToString();

    const int len = str->Utf8Length();
    char* buf = new char[len];
    str->WriteUtf8(buf, len);

    ResponsePtr res(new Response());
    res->set_data(buf, len);
    ret_setter(res);
    
    delete[] buf;
}

void Server::handle_message(struct mg_connection *conn) {
    if (on_message_.IsEmpty()) {
        return;
    }
    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate_, on_message_);
    if (!func->IsCallable()) {
        return;
    }

    v8::Isolate* isolate = isolate_; 
    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);

    //v8::Handle<v8::Value> params[2];
    //params[0] = v8::Integer::New(isolate_, 10);
    //params[1] = v8::Integer::New(isolate_, 2);

    //v8::Local<v8::Object> object = v8::Local<v8::Object>::New(isolate_, self_);
    //func->Call(object, 1, params);




    std::map<mg_connection*, mg_connection*>::iterator itr = websockets_.find(conn);
    if (itr == websockets_.end()) {
        websockets_[conn] = conn;
    }

    switch (conn->wsbits & 0x0f) {
    case 0x00:
        // denotes a continuation frame
        break;
    case 0x01:
        {
            v8::Handle<v8::Value> params[1];
            params[0] = v8::String::NewFromUtf8(isolate_, conn->content, v8::String::kNormalString, conn->content_len);
            v8::Local<v8::Object> object = v8::Local<v8::Object>::New(isolate_, self_);
            func->Call(object, 1, params);

            //mg_websocket_write(conn, 0x1, conn->content, conn->content_len);
        }
        break;
    case 0x02:
        // denotes a binary frame
        break;
    case 0x08:
        // denotes a connection close
        {
            std::map<mg_connection*, mg_connection*>::iterator itr = websockets_.find(conn);
            if (itr != websockets_.end()) {
                websockets_.erase(conn);
            }
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

void Server::handle_error(void) {
    if (on_error_.IsEmpty()) {
        return;
    }
    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate_, on_error_);
    if (func->IsCallable()) {
        return;
    }

    v8::Isolate* isolate = isolate_; 
    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);

    v8::Handle<v8::Value> params[2];
    params[0] = v8::Integer::New(isolate_, 10);
    params[1] = v8::Integer::New(isolate_, 2);

    v8::Local<v8::Object> object = v8::Local<v8::Object>::New(isolate_, self_);
    func->Call(object, 1, params);
}

int Server::request_handler(struct mg_connection *conn, enum mg_event ev) {

    if (ev == MG_REQUEST) {
        Server* s = static_cast<Server*>(conn->server_param);
        if (conn->is_websocket) {
            s->FireMessage(conn);
        } else {
            ResponsePtr res = s->FireRequest(conn);
            res->Send(conn);
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