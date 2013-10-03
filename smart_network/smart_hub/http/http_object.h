#ifndef HTTP_OBJECT_H_
#define HTTP_OBJECT_H_

#include <boost/thread.hpp>
#include <set>
#include <string>
#include <stdint.h>


struct mg_context;
struct mg_connection;

class HttpObject {
protected:
	HttpObject(const std::string& document_root, uint32_t port);
	~HttpObject(void);

public:
    typedef boost::function<int (struct mg_connection*, const char*, const char*)> RequestHandler;


    int  OnBeginRequest(mg_connection* conn);
    void OnEndRequest(const struct mg_connection* conn, int reply_status_code);
    int  OnWebsocketConnect(const struct mg_connection* conn);
    void OnWebsocketReady(struct mg_connection* conn);
    int  OnWebsocketData(struct mg_connection* conn, int bits, char* data, size_t data_len);

    void BindCommonHandler(RequestHandler handler);
    void UnbindCommonHandler(void);

    void PingWebSockets(void);

    void FireEvent(const std::string& json);

    struct mg_context* context(void) const;
    void set_context(struct mg_context* ctx);

    const char* document(void) const;
    uint32_t port(void) const;

private:
    virtual bool DoRequest(mg_connection* conn
                           , const char* method
                           , const char* query) = 0;
    virtual bool DoExecute(mg_connection* conn
                           , const char* method
                           , const char* uri) = 0;


private:
    struct mg_context* httpd_;
    const std::string document_;
    const uint32_t port_;

    mutable boost::mutex mutex_;
    std::set<struct mg_connection*> websockets_;

    RequestHandler common_handler_;
};

#endif  // HTTP_OBJECT_H_