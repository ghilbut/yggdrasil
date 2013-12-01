#ifndef HTTP_UI_OBJECT_H_
#define HTTP_UI_OBJECT_H_
#include "http_object.h"
#include "codebase/boost_lib_fwd.h"


struct mg_connection;

namespace Http {

class UIObject : public Object {
public:
    typedef boost::function<bool (const char*, std::string& filepath)> CommonPathHablder;
    void BindCommonPathHandler(CommonPathHablder handle);
    void UnbindCommonPathHandler(void);

    void FireEvent(const std::string& json);


protected:
    UIObject(const std::string& document_root, uint32_t port);
    virtual ~UIObject(void) {}

    virtual int OnRequest(mg_connection* conn);
    virtual void OnWebsocketText(const std::string& data);


private:
    virtual bool DoExecute(mg_connection* conn
                           , const char* method
                           , const char* query) = 0;
    virtual bool DoRequest(mg_connection* conn
                           , const char* method
                           , const char* query) = 0;
    virtual bool DoNotify(const std::string& text) = 0;


private:
    CommonPathHablder common_path_handle_;
};

}  // namespace Http

#endif  // HTTP_UI_OBJECT_H_