#ifndef DEVICE_POOL_H_
#define DEVICE_POOL_H_

#include <map>


class DeviceInfo;

class DevicePool {
public:
    DevicePool(const std::string& description_root);
    ~DevicePool(void);

    const DeviceInfo* operator[] (const std::string& id) const;


private:
    typedef std::map<std::string, const DeviceInfo*> DeviceMap;
    DeviceMap devices_;
};


#endif  // DEVICE_POOL_H_