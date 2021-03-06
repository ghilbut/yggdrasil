#ifndef SERVICE_FACTORY_H_
#define SERVICE_FACTORY_H_

#include <map>
#include <string>
#include <stdint.h>


class DeviceManager;
class ServiceBroker;

class ServiceFactory {
    enum { kPortBegin = 7000 };


public:
    typedef std::map<const std::string, ServiceBroker*> ServiceList;
    typedef ServiceList::iterator Iterator;


public:
    ServiceFactory(const DeviceManager& device_fac, const std::string& description_root);
    ~ServiceFactory(void);

    ServiceBroker* GetOrCreate(const std::string& id);
    void Remove(const std::string& id);

    void StartAll(void);
    void StopAll(void);

    ServiceBroker* operator[] (const std::string& id);
    Iterator Begin(void);
    Iterator End(void);


private:
    const DeviceManager& device_manager_;
    const std::string& description_root_;
    ServiceList service_list_;
    uint32_t next_port_;
};

#endif  // SERVICE_FACTORY_H_