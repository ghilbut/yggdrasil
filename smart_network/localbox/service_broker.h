#ifndef SERVICE_BROKER_H_
#define SERVICE_BROKER_H_

#include <string>
#include <stdint.h>
#include <v8.h>


class Storage;

class ServiceBroker {
public:
    ServiceBroker(v8::Isolate* isolate, const Storage& storage);
    ~ServiceBroker(void);

    void RunShell(void);

private:
    class Impl;
    Impl* pimpl_;
};

#endif  // SERVICE_BROKER_H_