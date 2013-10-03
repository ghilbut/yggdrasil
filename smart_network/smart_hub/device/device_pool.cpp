#include "device_pool.h"
#include "device_info.h"


DevicePool::DevicePool(const std::string& rootpath)
    : rootpath_(rootpath) {

    boost::filesystem::directory_iterator itr(rootpath_);
    boost::filesystem::directory_iterator end;
    for (; itr != end; ++itr) {
        const boost::filesystem::path& path = itr->path();

        const DeviceInfo* info = DeviceInfo::Create(path.string());
        if (info == NULL) {
            // TODO(jh81.kim): error handling.
            continue;
        }
        devices_[info->id()] = info;
    }
}

DevicePool::~DevicePool(void) {
    DeviceMap::const_iterator itr = devices_.begin();
    DeviceMap::const_iterator end = devices_.end();
    for (; itr != end; ++itr) {
        DeviceInfo::Delete(itr->second);
    }
}

const DeviceInfo* DevicePool::operator[] (const char* id) const {
    DeviceMap::const_iterator itr = devices_.find(id);
    return (itr != devices_.end() ? itr->second : NULL);
}