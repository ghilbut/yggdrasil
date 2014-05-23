#include "network_manager.h"


NetworkManager::NetworkManager(boost::asio::io_service& io_service) 
    : ssdp_scheduler_(io_service) {
    ssdp_scheduler_.BindTrigger(boost::bind(&NetworkManager::SendSsdp, this));
}

NetworkManager::~NetworkManager(void) {
    ssdp_scheduler_.UnbindTrigger();
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

void NetworkManager::RegisterSsdpTarget(const std::string& protocol, const std::string& target) {
    NetworkAdapterMap::iterator itr = net_adapters_.find(protocol);
    if (itr != net_adapters_.end()) {
        NetworkAdapter::Ptr adapter = itr->second;
        adapter->RegistTarget(target);
    }
}

void NetworkManager::UnregisterSsdpTarget(const std::string& protocol, const std::string& target) {
    NetworkAdapterMap::iterator itr = net_adapters_.find(protocol);
    if (itr != net_adapters_.end()) {
        NetworkAdapter::Ptr adapter = itr->second;
        adapter->UnregistTarget(target);
    }
}

void NetworkManager::SendSsdp(void) const {
    if (!net_adapters_.empty()) {
        NetworkAdapterMap::const_iterator itr = net_adapters_.begin();
        NetworkAdapterMap::const_iterator end = net_adapters_.end();
        for (; itr != end; ++itr) {
            NetworkAdapter::Ptr adapter = itr->second;
            adapter->SendSsdp();
        }
    }
}

void NetworkManager::Start(void) {
    ssdp_scheduler_.Start();
}

void NetworkManager::Stop(void) {
    ssdp_scheduler_.Stop();
}