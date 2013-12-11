#ifndef HTTP_UI_OBJECT_H_
#define HTTP_UI_OBJECT_H_

#include "http_websocket_manager.h"
#include "http_object.h"
#include "boost_lib_fwd.h"


namespace Http {

class UIObject : public Object {
protected:
    UIObject(const std::string& document_root, uint32_t port);
    virtual ~UIObject(void) {}


public:
    typedef boost::function<bool (const char*, std::string& filepath)> CommonPathHandler;
    void BindCommonPathHandler(CommonPathHandler handle);
    void UnbindCommonPathHandler(void);

    void FireEvent(const std::string& json);

    void SendWebSocketData(const std::string& data);
    void PingWebSockets(void);

    // Http::Object
    virtual int  OnBeginRequest(struct mg_connection* conn);
    virtual void OnEndRequest(const struct mg_connection* conn, int reply_status_code);
    virtual int  OnWebsocketConnect(const struct mg_connection* conn);
    virtual void OnWebsocketReady(struct mg_connection* conn);
    virtual int  OnWebsocketData(struct mg_connection* conn, int bits, char* data, size_t data_len);


private:
    virtual bool DoExecute(mg_connection* conn
                           , const char* method
                           , const char* query) = 0;
    virtual bool DoRequest(mg_connection* conn
                           , const char* method
                           , const char* query) = 0;
    virtual bool DoNotify(const std::string& text) = 0;


private:
    WebsocketManager websockets_;
    CommonPathHandler common_path_handle_;
};

}  // namespace Http

#endif  // HTTP_UI_OBJECT_H_