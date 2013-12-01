#include "service_factory.h"
#include "service_proxy.h"
#include "service/service_desc.h"
#include "codebase/device/device_manager.h"
#include "codebase/boost_lib_fwd.h"


ServiceFactory::ServiceFactory(const DeviceManager& device_fac, const std::string& description_root)
    : device_manager_(device_fac)
    , description_root_(description_root)
    , next_port_(kPortBegin) {

    boost::filesystem::directory_iterator itr(description_root_);
    boost::filesystem::directory_iterator end;
    for (; itr != end; ++itr) {
        const boost::filesystem::path& path = itr->path();

        ServiceDesc* service_desc = ServiceDesc::Create(path.string());
        if (service_desc == NULL) {
            // TODO(ghilbut): error handling.
            continue;
        }

        const DeviceDesc* device_desc = device_manager_[service_desc->device()];
        if (device_desc == NULL) {
            // TODO(ghilbut): error handling.
            ServiceDesc::Delete(service_desc);
            continue;
        }

        ServiceProxy* service = new ServiceProxy(*device_desc, *service_desc, ++next_port_);
        if (service == NULL) {
            // TODO(ghilbut): error handling.
            ServiceDesc::Delete(service_desc);
            continue;
        }

        service_list_[service_desc->id()] = service;
    }
}

ServiceFactory::~ServiceFactory(void) {
    ServiceList::iterator itr = service_list_.begin();
    ServiceList::iterator end = service_list_.end();
    for (; itr != end; ++itr) {
        delete itr->second;
    }
}

ServiceProxy* ServiceFactory::GetOrCreate(const std::string& id) {
    ServiceList::iterator itr = service_list_.find(id);
    if (itr != service_list_.end()) {
        return itr->second;
    }

    const std::string filename = id + ".txt";
    const std::string desc_path((boost::filesystem::path(description_root_) / filename).string());
    ServiceDesc* service_desc = ServiceDesc::Create(desc_path); // TODO(ghilbut): shared_ptr needed.
    if (service_desc == 0) {
        printf("[ERROR] no service(%s) description exists.\n", id.c_str());
        return 0;
    }

    const DeviceDesc* device_desc = device_manager_[service_desc->device()];
    if (device_desc == 0) {
        ServiceDesc::Delete(service_desc);
        printf("[ERROR] no device(%s) description exists.\n", id.c_str());
        return 0;
    }

    ServiceProxy* service = new ServiceProxy(*device_desc, *service_desc, next_port_++);
    if (service == 0) {
        ServiceDesc::Delete(service_desc);
        printf("[ERROR] service(%s) creation failed.\n", id.c_str());
        return 0;
    }

    // TODO(ghilbut): initialize service proxy
    return service;
}

void ServiceFactory::Remove(const std::string& id) {
    ServiceList::iterator itr = service_list_.find(id);
    BOOST_ASSERT(itr != service_list_.end());
    if (itr != service_list_.end()) {
        delete itr->second;
        service_list_.erase(itr);
    }
}

ServiceProxy* ServiceFactory::operator[] (const std::string& id) {
    return service_list_[id];
}

ServiceFactory::Iterator ServiceFactory::Begin(void) {
    return service_list_.begin();
}

ServiceFactory::Iterator ServiceFactory::End(void) {
    return service_list_.end();
}