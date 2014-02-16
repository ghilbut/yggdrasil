#ifndef HTTP_OBJECT_H_
#define HTTP_OBJECT_H_

#include "protocol_layer.h"
#include <boost/atomic.hpp>
#include <boost/thread.hpp>


class Service;

namespace Http {

class Object : public ProtocolLayer {
public:
    Object(void);
    ~Object(void);

    bool Start(void);
    void Stop(void);


    // ProtocolLayer
    virtual void OnRequest() {
        // nothing
    }
    virtual void OnResponse() {}
    virtual void OnNotify() {
        // nothing
    }
    virtual void OnEvent() {}


private:
    void thread_main(void);


private:
    boost::atomic<bool> alive_;
    boost::atomic<bool> stop_;
    boost::thread thread_;

public:
    Service* p;
};

}

#endif  // HTTP_OBJECT_H_