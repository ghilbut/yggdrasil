#ifndef NETWORK_MANAGER_H_
#define NETWORK_MANAGER_H_

#include "ssdp_scheduler.h"
#include "network_adapter.h"
#include <string>
#include <map>


class NetworkManager : public boost::noncopyable {
public:
    explicit NetworkManager(IOService& io_service);
    ~NetworkManager(void);

    bool IsAlreadyRegistered(const std::string& protocol);
    void Register(const std::string& protocol, NetworkAdapter::Ptr& adapter);
    void Unregister(const std::string& protocol);

    void RegisterSsdpTarget(const std::string& protocol, const std::string& target);
    void UnregisterSsdpTarget(const std::string& protocol, const std::string& target);
    void SendSsdp(void) const;

    void Start(void);
    void Stop(void);


private:
    typedef std::map<std::string, NetworkAdapter::Ptr> NetworkAdapterMap;
    NetworkAdapterMap net_adapters_;
    Ssdp::Scheduler ssdp_scheduler_;
};

#endif  // NETWORK_MANAGER_H_