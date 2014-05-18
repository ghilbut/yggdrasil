#ifndef CHANNEL_H_
#define CHANNEL_H_

#include "base/ref_implement.h"
#include <v8.h>


class Environ;

class Channel : public RefImplement {
public:
    Channel(Environ& env);
    ~Channel(void);

    void FireServiceLoaded();
    void FireChannelOpend();

    v8::Local<v8::Object> self(v8::Isolate* isolate) const;


private:
    Environ& env_;
    v8::Persistent<v8::Object> self_;
};

#endif  // CHANNEL_H_