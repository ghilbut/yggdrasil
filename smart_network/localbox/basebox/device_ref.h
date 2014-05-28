#ifndef DEVICE_REF_H_
#define DEVICE_REF_H_

#include <string>


class Device;
class IOServiceRef;

class DeviceRef {
public:
    DeviceRef(void);
    DeviceRef(const IOServiceRef& io_service, const std::string& basepath);
    DeviceRef(const DeviceRef& other);
    ~DeviceRef(void);

    DeviceRef& operator= (const DeviceRef& other);
    bool operator== (const DeviceRef& other) const;
    bool operator!= (const DeviceRef& other) const;

    Device* operator-> (void) const;

    void Reset(Device* device);
    bool IsNull(void) const;


private:
    Device* impl_;
};

#endif  // DEVICE_REF_H_