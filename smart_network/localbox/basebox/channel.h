#ifndef CHANNEL_H_
#define CHANNEL_H_

#include "base/ref_object.h"
#include <v8.h>


class Device;

class Channel : public RefObject {
public:
    Channel(Device& device);
    ~Channel(void);

    void FireServiceLoaded();
    void FireChannelOpend();

    v8::Local<v8::Object> self(v8::Isolate* isolate) const;


private:
    Device& device_;
    v8::Persistent<v8::Object> self_;
};

#endif  // CHANNEL_H_