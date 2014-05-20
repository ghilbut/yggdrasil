#ifndef DEVICE_REF_H_
#define DEVICE_REF_H_


class Device;
class IOServiceRef;

class DeviceRef {
public:
    DeviceRef(void);
    DeviceRef(const IOServiceRef& io_service, const char* basepath);
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