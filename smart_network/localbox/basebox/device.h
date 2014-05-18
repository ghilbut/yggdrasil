#ifndef DEVICE_H_
#define DEVICE_H_

#include "base/ref_implement.h"
#include <v8.h>


class Environ;

class Device : public RefImplement {
public:
    Device(Environ& env);
    ~Device(void);

    void FireServiceLoaded();
    void FireChannelOpend();


public: // for javascript template
    v8::Local<v8::Object> self(v8::Isolate* isolate) const;
    v8::Local<v8::Object> service_load_trigger(v8::Isolate* isolate) const;
    void set_service_load_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);
    v8::Local<v8::Object> channel_open_trigger(v8::Isolate* isolate) const;
    void set_channel_open_trigger(v8::Isolate* isolate, v8::Handle<v8::Object>& trigger);


private:
    Environ& env_;
    v8::Persistent<v8::Object> self_;
    v8::Persistent<v8::Object> on_service_load_;
    v8::Persistent<v8::Object> on_channel_open_;
};

#endif  // DEVICE_H_