#ifndef SERVICE_MANAGER_H_
#define SERVICE_MANAGER_H_

#include "service_ref.h"
#include "device_manager.h"
#include "base/io_service_ref.h"
#include <map>
#include <string>


class ServiceManagerDelegate;

class ServiceManager {
public:
    ServiceManager(DeviceManager& device_manager, ServiceManagerDelegate& delegate, const std::string& service_root);
    ~ServiceManager(void);

    ServiceRef& operator[] (const std::string& id);


private:
    typedef std::map<std::string, ServiceRef> ServiceMap;
    ServiceMap services_;

    DeviceManager& device_manager_;
    ServiceManagerDelegate& delegate_;
};

#endif  // SERVICE_MANAGER_H_