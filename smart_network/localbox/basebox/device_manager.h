#ifndef DEVICE_MANAGER_H_
#define DEVICE_MANAGER_H_

#include "device_ref.h"
#include "base/io_service_ref.h"
#include <map>
#include <string>


class DeviceManager {
public:
    DeviceManager(const IOServiceRef& io_service, const std::string& device_root);
    ~DeviceManager(void);

    DeviceRef& operator[] (const std::string& id);


private:
    IOServiceRef io_service_;
    typedef std::map<std::string, DeviceRef> DeviceMap;
    DeviceMap devices_;
};

#endif  // DEVICE_MANAGER_H_