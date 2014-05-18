#ifndef SERVICE_H_
#define SERVICE_H_

#include "base/ref_implement.h"
#include <v8.h>


class Environ;

class Service : public RefImplement {
public:
    Service(Environ& env);
    ~Service(void);


private:
    Environ& env_;
    v8::Persistent<v8::Object> self_;
};

#endif  // SERVICE_H_