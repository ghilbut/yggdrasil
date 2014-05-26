#include "device_manager.h"
#include "device_desc.h"
#include "device.h"
#include <boost/filesystem.hpp>


DeviceManager::DeviceManager(const IOServiceRef& io_service, const std::string& device_root) {

    boost::filesystem::path root(device_root);
    boost::filesystem::directory_iterator itr(root);
    boost::filesystem::directory_iterator end;
    for (; itr != end; ++itr) {
        const boost::filesystem::path& path = itr->path();
        /*const DeviceDesc* info = DeviceDesc::Create(path.string());
        if (info == NULL) {
            // TODO(ghilbut): error handling.
            continue;
        }*/
        DeviceRef device(io_service, path.string().c_str());
        //devices_[info->id()] = DeviceRef(io_service, path.string().c_str());
        //devices_[info->id()] = DeviceRef(io_service, device_root.c_str());
        //devices_[info->id()] = info;
        if (device.IsNull()) {
            // TODO(ghilbut): error handling.
            continue;
        }
        devices_[device->id()] = device;
        //DeviceDesc::Delete(info);
    }
}

DeviceManager::~DeviceManager(void) {
    DeviceMap::const_iterator itr = devices_.begin();
    DeviceMap::const_iterator end = devices_.end();
    for (; itr != end; ++itr) {
        //DeviceDesc::Delete(itr->second);
    }
}

DeviceRef& DeviceManager::operator[] (const std::string& id) {
    DeviceMap::iterator itr = devices_.find(id);
    return (itr != devices_.end() ? itr->second : DeviceRef());
}
