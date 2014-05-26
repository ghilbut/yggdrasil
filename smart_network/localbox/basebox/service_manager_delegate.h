#ifndef SERVICE_MANAGER_DELEGATE_H_
#define SERVICE_MANAGER_DELEGATE_H_

class ServiceRef;


class ServiceManagerDelegate {
public:
    virtual void OnServiceOpen(const ServiceRef& service) = 0;
    virtual void OnServiceClosed(const ServiceRef& service) = 0;

protected:
    virtual ~ServiceManagerDelegate(void) {}
};

#endif  // SERVICE_MANAGER_DELEGATE_H_