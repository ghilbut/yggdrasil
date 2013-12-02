#ifndef TROMM_H_
#define TROMM_H_

#include "device.h"


class Tromm : public Device {
public:
    typedef enum {
        kMode00 = 0, kMode01, kMode02, kMode03, kMode04, 
        kMode05, kMode06, kMode07, kMode08, kMode09, 
        kMode10, kMode11, kMode12, kMode13
    } Mode;

public:
    Tromm(boost::asio::io_service& io_service, const std::string& description);
    ~Tromm(void);

    virtual void OnConnected(Event& event);
    virtual void OnRequest(const Request& req, Response& res);
    virtual void OnNotify(const std::string& text);
    virtual void OnError(void);
    virtual void OnDisconnected(void);


private:
    bool is_power_on_;
    bool is_running_;
    Mode mode_;    
};

#endif  // TROMM_H_