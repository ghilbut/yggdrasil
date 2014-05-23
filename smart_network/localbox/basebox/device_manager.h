#ifndef DEVICE_MANAGER_H_
#define DEVICE_MANAGER_H_

#include "device_ref.h"
#include "base/io_service_ref.h"
#include <map>
#include <string>


class DeviceDesc;

class DeviceManager {
public:
    DeviceManager(const IOServiceRef& io_service, const std::string& device_root);
    ~DeviceManager(void);

    DeviceRef& operator[] (const std::string& id);
    //const DeviceDesc* operator[] (const std::string& id) const;


private:
    IOServiceRef io_service_;
    typedef std::map<std::string, DeviceRef> DeviceMap;
    //typedef std::map<std::string, const DeviceDesc*> DeviceMap;
    DeviceMap devices_;
};

#endif  // DEVICE_MANAGER_H_