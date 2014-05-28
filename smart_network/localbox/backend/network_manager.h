#ifndef NETWORK_MANAGER_H_
#define NETWORK_MANAGER_H_

#include "ssdp_scheduler.h"
#include "network_adapter.h"
#include <string>
#include <map>


class NetworkManager : public boost::noncopyable {
public:
    explicit NetworkManager(boost::asio::io_service& io_service);
    ~NetworkManager(void);

    bool IsAlreadyRegistered(const std::string& protocol);
    void Register(const std::string& protocol, NetworkAdapter::Ptr& adapter);
    void Unregister(const std::string& protocol);

    void SendSsdp(const char* target) const;


private:
    typedef std::map<std::string, NetworkAdapter::Ptr> NetworkAdapterMap;
    NetworkAdapterMap net_adapters_;
};

#endif  // NETWORK_MANAGER_H_