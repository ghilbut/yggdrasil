#ifndef DEVICE_REF_H_
#define DEVICE_REF_H_


class DeviceContext;
class DeviceHost;

class DeviceRef {
public:
    DeviceRef(void);
    explicit DeviceRef(DeviceContext& context);
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

#endif  // DEVICE_REF_H_