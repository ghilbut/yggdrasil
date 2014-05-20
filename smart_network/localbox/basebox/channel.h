#ifndef CHANNEL_H_
#define CHANNEL_H_

#include "base/ref_implement.h"
#include <v8.h>


class DeviceHost;

class Channel : public RefImplement {
public:
    Channel(DeviceHost& device);
    ~Channel(void);

    void FireServiceLoaded();
    void FireChannelOpend();

    v8::Local<v8::Object> self(v8::Isolate* isolate) const;


private:
    DeviceHost& device_;
    v8::Persistent<v8::Object> self_;
};

#endif  // CHANNEL_H_