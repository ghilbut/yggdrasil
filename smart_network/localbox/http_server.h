#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include "environ.h"
#include "http_websocket_manager.h"
#include <v8.h>
#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <map>
#include <stdint.h>


struct mg_server;
struct mg_connection;
enum mg_event;

class Environ;

namespace Http {

class Request;
class Response;
class Message;

class Server {
public:
    Server(Environ* env);
    ~Server(void);

    static void WeakCallback(const v8::WeakCallbackData<v8::Object, Server>& data);
    void MakeWeak(v8::Isolate* isolate, v8::Local<v8::Object> self);
    void ClearWeak(void);

    bool DoListen(void);
    void DoClose(void);

    void DoNotify(const Message& msg);

    v8::Local<v8::Function> request_trigger(v8::Isolate* isolate) const;
    void set_request_trigger(v8::Isolate* isolate, v8::Handle<v8::Function> trigger);
    v8::Local<v8::Object> open_trigger(v8::Isolate* isolate) const;
    void set_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object> trigger);
    v8::Local<v8::Function> error_trigger(v8::Isolate* isolate) const;
    void set_error_trigger(v8::Isolate* isolate, v8::Handle<v8::Function> trigger);

private:
    Response FireRequest(struct mg_connection *conn);
    void FireError(void);

    void handle_listen(boost::function<void(const bool&)> ret_setter);
    void handle_close(void);

    void handle_request(struct mg_connection *conn, boost::function<void(const Response&)> ret_setter);
    void handle_error(void);

    static int request_handler(struct mg_connection *conn, enum mg_event ev);
    void thread_main(void);

private:
    Environ* env_;

    uint16_t port_;

    v8::Isolate* isolate_;
    v8::Persistent<v8::Object> self_;
    v8::Persistent<v8::Function> on_request_;
    v8::Persistent<v8::Function> on_error_;

    boost::asio::strand strand_;
    boost::atomic_bool is_alive_;
    boost::atomic_bool is_stop_;
    boost::thread thread_;

    WebSocketManager ws_manager_;
};

}  // namespace Http

#endif  // HTTP_SERVER_H_