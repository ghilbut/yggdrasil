#ifndef SERVICE_H_
#define SERVICE_H_

#include "base/ref_implement.h"
#include <v8.h>


class DeviceContext;

class Service : public RefImplement {
public:
    Service(DeviceContext& context);
    ~Service(void);


private:
    DeviceContext& context_;
    v8::Persistent<v8::Object> self_;
};

#endif  // SERVICE_H_