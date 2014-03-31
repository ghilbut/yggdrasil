#ifndef SERVICE_BROKER_H_
#define SERVICE_BROKER_H_

#include <string>
#include <stdint.h>


class Storage;

class ServiceBroker {
public:
    ServiceBroker(const Storage& storage);
    ~ServiceBroker(void);

    void RunShell(void);

private:
    class Impl;
    Impl* pimpl_;
};

#endif  // SERVICE_BROKER_H_