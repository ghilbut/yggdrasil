#ifndef DEVICE_REF_H_
#define DEVICE_REF_H_


class Environ;
class Device;

class DeviceRef {
public:
    DeviceRef(void);
    explicit DeviceRef(Environ& env);
    explicit DeviceRef(const DeviceRef& other);
    ~DeviceRef(void);

    void Reset(Device* device);

    DeviceRef& operator= (const DeviceRef& other);
    bool operator== (const DeviceRef& other) const;
    bool operator!= (const DeviceRef& other) const;

    Device* operator-> (void) const;


private:
    Device* impl_;
};

#endif  // DEVICE_REF_H_