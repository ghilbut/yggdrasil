#include "device_manager.h"
#include "device_desc.h"
#include "boost_lib_fwd.h"


DeviceManager::DeviceManager(const std::string& description_root) {

    boost::filesystem::path root(description_root);
    boost::filesystem::directory_iterator itr(root);
    boost::filesystem::directory_iterator end;
    for (; itr != end; ++itr) {
        const boost::filesystem::path& path = itr->path();
        const DeviceDesc* info = DeviceDesc::Create(path.string());
        if (info == NULL) {
            // TODO(ghilbut): error handling.
            continue;
        }
        devices_[info->id()] = info;
    }
}

DeviceManager::~DeviceManager(void) {
    DeviceMap::const_iterator itr = devices_.begin();
    DeviceMap::const_iterator end = devices_.end();
    for (; itr != end; ++itr) {
        DeviceDesc::Delete(itr->second);
    }
}

const DeviceDesc* DeviceManager::operator[] (const std::string& id) const {
    DeviceMap::const_iterator itr = devices_.find(id);
    return (itr != devices_.end() ? itr->second : NULL);
}