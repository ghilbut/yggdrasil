#ifndef TCP_ADAPTER_H_
#define TCP_ADAPTER_H_

#include "network_adapter.h"
#include "ssdp_ethernet_sender.h"
#include "tcp_channel.h"
#include "base/io_service_ref.h"
#include <string>


class Channel;
class ConnectionDelegate;

class TcpAdapter : public NetworkAdapter {
public:
    TcpAdapter(const IOServiceRef& io_service, ChannelDelegate* delegate, unsigned short port = 8091);
    ~TcpAdapter(void);

    // NetworkAdapter
    virtual void SendSsdp(const char* target) const;


private:
    void DoListen(void);
    void handle_accept(Channel* channel, const boost::system::error_code& error);


private:
    IOServiceRef io_service_;
    TCP::acceptor acceptor_;
    ChannelDelegate* delegate_;

    Ssdp::EthernetSender ssdp_sender_;
};


#endif  // TCP_ADAPTER_H_