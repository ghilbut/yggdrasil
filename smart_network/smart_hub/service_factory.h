#ifndef SERVICE_FACTORY_H_
#define SERVICE_FACTORY_H_

#include <map>
#include <string>
#include <stdint.h>


class DevicePool;
class ServiceProxy;

class ServiceFactory {
    enum { kPortBegin = 7000 };


public:
    typedef std::map<const std::string, ServiceProxy*> ServiceList;
    typedef ServiceList::iterator Iterator;


public:
    ServiceFactory(const DevicePool& device_fac, const std::string& description_root);
    ~ServiceFactory(void);

    ServiceProxy* GetOrCreate(const std::string& id);
    void Remove(const std::string& id);

    ServiceProxy* operator[] (const std::string& id);
    Iterator Begin(void);
    Iterator End(void);


private:
    const DevicePool& device_fac_;
    const std::string& description_root_;
    ServiceList service_list_;
    uint32_t next_port_;
};

#endif  // SERVICE_FACTORY_H_