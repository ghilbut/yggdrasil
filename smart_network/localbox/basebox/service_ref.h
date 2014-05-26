#ifndef SERVICE_REF_H_
#define SERVICE_REF_H_


class DeviceRef;
class ServiceDesc;
class Service;

class ServiceRef {
public:
    ServiceRef(void);
    ServiceRef(const DeviceRef& device, ServiceDesc* desc);
    ServiceRef(const ServiceRef& other);
    ~ServiceRef(void);

    ServiceRef& operator= (const ServiceRef& other);
    bool operator== (const ServiceRef& other) const;
    bool operator!= (const ServiceRef& other) const;

    Service* operator-> (void) const;

    Service* Get(void) const;
    void Reset(Service* service);
    bool IsNull(void) const;


private:
    Service* impl_;
};

#endif  // SERVICE_REF_H_