#include "service_manager.h"

#include "root_storage.h"
#include "service_desc.h"
#include <boost/filesystem.hpp>


ServiceManager::ServiceManager(const IOServiceRef& io_service, const RootStorage& storage)
    : io_service_(io_service)
    , device_manager_(io_service_, storage.device_base()) {

    boost::filesystem::directory_iterator itr(storage.service_base());
    boost::filesystem::directory_iterator end;
    for (; itr != end; ++itr) {
        const boost::filesystem::path& path = itr->path();

        ServiceDesc* service_desc = ServiceDesc::Create(path.string());
        if (service_desc == NULL) {
            // TODO(ghilbut): error handling.
            continue;
        }

        DeviceRef device = device_manager_[service_desc->device()];
        if (device.IsNull()) {
            // TODO(ghilbut): error handling.
            //ServiceDesc::Delete(service_desc);
            continue;
        }

        ServiceRef service(device);
        //ServiceBroker* service = new ServiceBroker(*device_desc, *service_desc, ++next_port_);
        if (service.IsNull()) {
            // TODO(ghilbut): error handling.
            //ServiceDesc::Delete(service_desc);
            continue;
        }

        services_[service_desc->id()] = service;
    }
}

ServiceManager::~ServiceManager(void) {

}