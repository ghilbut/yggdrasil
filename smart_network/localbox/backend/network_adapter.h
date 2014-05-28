#ifndef NETWORK_ADAPTER_H_
#define NETWORK_ADAPTER_H_

#include "boost_lib_fwd.h"

class NetworkAdapter {
public:
    typedef boost::shared_ptr<NetworkAdapter> Ptr;

    virtual void SendSsdp(const char* target) const = 0;

protected:
    virtual ~NetworkAdapter(void) {}
};

#endif  // NETWORK_ADAPTER_H_