#ifndef DEVICE_HOST_REF_H_
#define DEVICE_HOST_REF_H_


class DeviceHost;
class IOServiceRef;

class DeviceRef {
public:
    DeviceRef(void);
    DeviceRef(const IOServiceRef& io_service, const char* basepath);
    explicit DeviceRef(const DeviceRef& other);
    ~DeviceRef(void);

    void Reset(DeviceHost* device);

    DeviceRef& operator= (const DeviceRef& other);
    bool operator== (const DeviceRef& other) const;
    bool operator!= (const DeviceRef& other) const;

    DeviceHost* operator-> (void) const;


private:
    DeviceHost* impl_;
};

#endif  // DEVICE_HOST_REF_H_