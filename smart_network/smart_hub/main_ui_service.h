#ifndef MAIN_UI_SERVICE_H_
#define MAIN_UI_SERVICE_H_

#include "codebase/http_ui_object.h"


class MainUIHandler;

class MainUIService : public Http::UIObject {
public:
    MainUIService(const std::string& document_root, uint32_t port);
    ~MainUIService(void) {};

    void BindUIHandler(MainUIHandler* delegator);
    void UnbindUIHandler(void);


private:
    // Http::UIObject
    virtual bool FireRequest(mg_connection* conn
                             , const char* method
                             , const char* query);
    virtual bool FireNotify(const std::string& text);


private:
    MainUIHandler* ui_handler_;
};

#endif  // MAIN_UI_SERVICE_H_