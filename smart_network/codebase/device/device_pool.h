#ifndef DEVICE_POOL_H_
#define DEVICE_POOL_H_

#include <map>
#include <boost/filesystem.hpp>


class DeviceInfo;

class DevicePool {
public:
    DevicePool(const std::string& rootpath);
    ~DevicePool(void);

    const DeviceInfo* operator[] (const char* id) const;


private:
    typedef std::map<std::string, const DeviceInfo*> DeviceMap;
    DeviceMap devices_;

    const boost::filesystem::path rootpath_;
};


#endif  // DEVICE_POOL_H_