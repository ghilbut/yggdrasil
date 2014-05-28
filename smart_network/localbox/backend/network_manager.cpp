#include "network_manager.h"


NetworkManager::NetworkManager(boost::asio::io_service& io_service) {
    // nothing
}

NetworkManager::~NetworkManager(void) {
    // nothing
}

bool NetworkManager::IsAlreadyRegistered(const std::string& protocol) {
    return (net_adapters_.find(protocol) != net_adapters_.end());
}

void NetworkManager::Register(const std::string& protocol, NetworkAdapter::Ptr& adapter) {
    net_adapters_[protocol] = adapter;
}

void NetworkManager::Unregister(const std::string& protocol) {
    net_adapters_.erase(protocol);
}

void NetworkManager::SendSsdp(const char* target) const {
    if (!net_adapters_.empty()) {
        NetworkAdapterMap::const_iterator itr = net_adapters_.begin();
        NetworkAdapterMap::const_iterator end = net_adapters_.end();
        for (; itr != end; ++itr) {
            NetworkAdapter::Ptr adapter = itr->second;
            adapter->SendSsdp(target);
        }
    }
}
