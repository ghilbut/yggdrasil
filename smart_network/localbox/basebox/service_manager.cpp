#include "service_manager.h"

#include "root_storage.h"
#include "service_desc.h"
#include "service_manager_delegate.h"
#include <boost/filesystem.hpp>


ServiceManager::ServiceManager(DeviceManager& device_manager, ServiceManagerDelegate& delegate, const std::string& service_root)
    : device_manager_(device_manager)
    , delegate_(delegate) {

    boost::filesystem::directory_iterator itr(service_root);
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

        ServiceRef service(device, service_desc);
        //ServiceBroker* service = new ServiceBroker(*device_desc, *service_desc, ++next_port_);
        if (service.IsNull()) {
            // TODO(ghilbut): error handling.
            //ServiceDesc::Delete(service_desc);
            continue;
        }

        services_[service_desc->id()] = service;
        delegate_.OnServiceOpen(service);
    }
}

ServiceManager::~ServiceManager(void) {

}

ServiceRef& ServiceManager::operator[] (const std::string& id) {
    ServiceMap::iterator itr = services_.find(id);
    return (itr != services_.end() ? itr->second : ServiceRef());
}
