#ifndef HTTP_WEBSOCKET_MANAGER_H_
#define HTTP_WEBSOCKET_MANAGER_H_

#include <v8.h>
#include <boost/asio.hpp>
#include <map>


class Environ;
struct mg_connection;

namespace Http {

class Message;

class WebSocketManager {
public:
    WebSocketManager(Environ* env);
    ~WebSocketManager(void);

    void HandleMessage(mg_connection* conn);
    void DoNotify(const Message& msg);

    void set_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Function>& trigger);
    v8::Local<v8::Function> message_trigger(v8::Isolate* isolate) const;
    void set_message_trigger(v8::Isolate* isolate, v8::Handle<v8::Function>& trigger);
    void set_closed_trigger(v8::Isolate* isolate, v8::Handle<v8::Function>& trigger);

private:
    void event_open(mg_connection* conn);
    void event_message(mg_connection* conn, const Message& msg);
    void event_closed(mg_connection* conn);
    void trigger_notify(const Message& msg);


private:
    v8::Isolate* isolate_;
    v8::Persistent<v8::Object> caller_;
    v8::Persistent<v8::Function> on_open_;
    v8::Persistent<v8::Function> on_message_;
    v8::Persistent<v8::Function> on_closed_;
    boost::asio::strand strand_;

    typedef v8::CopyablePersistentTraits<v8::Object> Copyable;
    typedef v8::Persistent<v8::Object, Copyable> CopyablePersistent;
    typedef std::map<mg_connection*, CopyablePersistent> WebSocketMap;
    WebSocketMap websockets_;
};

}  // namespace Http

#endif  // HTTP_WEBSOCKET_MANAGER_H_