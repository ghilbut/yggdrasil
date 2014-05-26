#include "device_manager.h"

#include "device.h"
#include <boost/filesystem.hpp>


DeviceManager::DeviceManager(const IOServiceRef& io_service, const std::string& device_root) {

    boost::filesystem::path root(device_root);
    boost::filesystem::directory_iterator itr(root);
    boost::filesystem::directory_iterator end;
    for (; itr != end; ++itr) {
        const boost::filesystem::path& path = itr->path();
        DeviceRef device(io_service, path.string().c_str());
        if (device.IsNull()) {
            // TODO(ghilbut): error handling.
            continue;
        }
        devices_[device->id()] = device;
    }
}

DeviceManager::~DeviceManager(void) {
    // nothing
}

DeviceRef& DeviceManager::operator[] (const std::string& id) {
    DeviceMap::iterator itr = devices_.find(id);
    return (itr != devices_.end() ? itr->second : DeviceRef());
}
