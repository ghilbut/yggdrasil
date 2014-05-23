#ifndef SERVICE_MANAGER_H_
#define SERVICE_MANAGER_H_

#include "service_ref.h"
#include "device_manager.h"
#include "base/io_service_ref.h"
#include <map>
#include <string>


class RootStorage;

class ServiceManager {
public:
    ServiceManager(const IOServiceRef& io_service, const RootStorage& storage);
    ~ServiceManager(void);


private:
    typedef std::map<std::string, ServiceRef> ServiceMap;
    ServiceMap services_;

    IOServiceRef io_service_;
    DeviceManager device_manager_;
};

#endif  // SERVICE_MANAGER_H_