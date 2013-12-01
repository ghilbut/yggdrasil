#ifndef DEVICE_MANAGER_H_
#define DEVICE_MANAGER_H_

#include <map>


class DeviceDesc;

class DeviceManager {
public:
    DeviceManager(const std::string& description_root);
    ~DeviceManager(void);

    const DeviceDesc* operator[] (const std::string& id) const;


private:
    typedef std::map<std::string, const DeviceDesc*> DeviceMap;
    DeviceMap devices_;
};

#endif  // DEVICE_MANAGER_H_