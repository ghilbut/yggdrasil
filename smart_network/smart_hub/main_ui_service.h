#ifndef MAIN_UI_SERVICE_H_
#define MAIN_UI_SERVICE_H_

#include "http/http_ui_object.h"


class MainUIHandler;

class MainUIService : public Http::UIObject {
public:
    MainUIService(const std::string& document_root, uint32_t port);
    ~MainUIService(void) {};

    void BindUIHandler(MainUIHandler* delegator);
    void UnbindUIHandler(void);


private:
    // Http::UIObject
    virtual bool DoExecute(mg_connection* conn
                           , const char* method
                           , const char* query);
    virtual bool DoRequest(mg_connection* conn
                           , const char* method
                           , const char* query);
    virtual bool DoNotify(const std::string& text);


private:
    MainUIHandler* ui_handler_;
};

#endif  // MAIN_UI_SERVICE_H_