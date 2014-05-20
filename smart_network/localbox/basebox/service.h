#ifndef SERVICE_H_
#define SERVICE_H_

#include "base/ref_implement.h"
#include <v8.h>


class DeviceHost;

class Service : public RefImplement {
public:
    Service(DeviceHost& device);
    ~Service(void);


private:
    DeviceHost& device_;
    v8::Persistent<v8::Object> self_;
};

#endif  // SERVICE_H_