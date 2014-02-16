#ifndef SERVICE_BROKER_H_
#define SERVICE_BROKER_H_

#include "http_object.h"


class Channel;

class ServiceBroker : public ProtocolLayer {
public:
    ServiceBroker(void) {}
    ~ServiceBroker(void) {}

    void Start(void);
    void Stop(void);


    // ProtocolLayer
    virtual void OnRequest();
    virtual void OnResponse();
    virtual void OnNotify();
    virtual void OnEvent();


private:
    Http::Object httpd_;
    Channel* channel_;
};

#endif  // SERVICE_BROKER_H_