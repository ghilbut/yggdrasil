#ifndef SERVICE_REF_H_
#define SERVICE_REF_H_


class Device;
class Service;

class ServiceRef {
public:
    ServiceRef(void);
    explicit ServiceRef(Device& device);
    explicit ServiceRef(const ServiceRef& other);
    ~ServiceRef(void);

    void Reset(Service* service);

    ServiceRef& operator= (const ServiceRef& other);
    bool operator== (const ServiceRef& other) const;
    bool operator!= (const ServiceRef& other) const;

    Service* operator-> (void) const;


private:
    Service* impl_;
};

#endif  // SERVICE_REF_H_