 #include "service_pool.h"
#include "http/http_server.h"
#include "device/device_pool.h"
#include "service_info.h"
#include "service.h"
#include <boost/filesystem.hpp>


ServicePool::ServicePool(HttpServer& httpd
                         , const DevicePool& devices
                         , const std::string& rootpath)
    : httpd_(httpd)
    , devices_(devices)
    , rootpath_(rootpath)
    , next_port_ (7000) {

    boost::filesystem::directory_iterator itr(rootpath_);
    boost::filesystem::directory_iterator end;
    for (; itr != end; ++itr) {
        const boost::filesystem::path& path = itr->path();

        ServiceInfo* info = ServiceInfo::Create(path.string());
        if (info == NULL) {
            // TODO(jh81.kim): error handling.
            continue;
        }

        const DeviceInfo* device = devices[info->device()];
        if (device == NULL) {
            // TODO(jh81.kim): error handling.
            ServiceInfo::Delete(info);
            continue;
        }

        Service* service = new Service(device, info, next_port_++, NULL);
        if (service == NULL) {
            // TODO(jh81.kim): error handling.
            ServiceInfo::Delete(info);
            continue;
        }

        services_[info->id()] = service;
    }
}

ServicePool::~ServicePool(void) {
    ServiceMap::iterator itr = services_.begin();
    ServiceMap::iterator end = services_.end();
    for (; itr != end; ++itr) {
        delete itr->second;
    }
}

void ServicePool::Start(void) {

    ServiceMap::iterator itr = services_.begin();
    ServiceMap::iterator end = services_.end();
    for (; itr != end; ++itr) {
        HttpObject* http = static_cast<HttpObject*>(itr->second);
        if (!httpd_.Insert(http)) {
            // TODO(jh81.kim): error handling.
        }
    }
}

void ServicePool::Stop(void) {
    ServiceMap::iterator itr = services_.begin();
    ServiceMap::iterator end = services_.end();
    for (; itr != end; ++itr) {
        HttpObject* info = static_cast<HttpObject*>(itr->second);
        httpd_.Remove(info);
    }
}

ServicePool::Iterator ServicePool::Begin(void) {
    return services_.begin();
}

ServicePool::Iterator ServicePool::End(void) {
    return services_.end();
}

Service* ServicePool::operator[] (const char* id) const {
    ServiceMap::const_iterator itr = services_.find(id);
    return (itr != services_.end() ? itr->second : NULL);
}