#ifndef MAIN_UI_HANDLER_H_
#define MAIN_UI_HANDLER_H_

#include <string>


class MainUIHandler {
public:
    virtual void handle_get_device_list(std::string& json) = 0; // let's const. but not yet, because ServiceFactory iterator.


protected:
    ~MainUIHandler(void) {}
};

#endif  // MAIN_UI_HANDLER_H_