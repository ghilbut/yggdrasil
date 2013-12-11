#ifndef HTTP_OBJECT_H_
#define HTTP_OBJECT_H_

#include "boost_lib_fwd.h"
#include <set>
#include <string>
#include <stdint.h>


struct mg_context;
struct mg_connection;

namespace Http {

class Object : boost::noncopyable {
protected:
    Object(const std::string& document_root, uint32_t port);
    virtual ~Object(void) {}


public:
    // mongoose
    virtual int  OnBeginRequest(struct mg_connection* conn) = 0;
    virtual void OnEndRequest(const struct mg_connection* conn, int reply_status_code) = 0;
    virtual int  OnWebsocketConnect(const struct mg_connection* conn) = 0;
    virtual void OnWebsocketReady(struct mg_connection* conn) = 0;
    virtual int  OnWebsocketData(struct mg_connection* conn, int bits, char* data, size_t data_len) = 0;

    bool Start(void);
    void Stop(void);

    uint32_t port(void) const;


private:
    mg_context* ctx_;
    const std::string document_root_;
    const uint32_t port_;
};

}  // namespace Http

#endif  // HTTP_OBJECT_H_