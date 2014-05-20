#ifndef SERVICE_H_
#define SERVICE_H_

#include "base/ref_implement.h"
#include <v8.h>


class Device;

class Service : public RefImplement {
public:
    Service(Device& device);
    ~Service(void);


private:
    Device& device_;
    v8::Persistent<v8::Object> self_;
};

#endif  // SERVICE_H_