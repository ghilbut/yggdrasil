#ifndef TCP_ADAPTER_H_
#define TCP_ADAPTER_H_

#include "network_adapter.h"
#include "ssdp_ethernet_sender.h"
#include "tcp_channel.h"
#include <string>


class ConnectionDelegate;

class TcpAdapter : public NetworkAdapter {
public:
    TcpAdapter(IOService& io_service, ChannelDelegate* delegate, unsigned short port = 8091);
    ~TcpAdapter(void);

    // NetworkAdapter
    virtual void RegistTarget(const std::string& target);
    virtual void UnregistTarget(const std::string& target);
    virtual void SendSsdp(void) const;


private:
    void DoListen(void);
    void handle_accept(TcpChannel* channel, const boost::system::error_code& error);


private:
    IOService& io_service_;
    TCP::acceptor acceptor_;
    ChannelDelegate* delegate_;

    Ssdp::EthernetSender ssdp_sender_;
};


#endif  // TCP_ADAPTER_H_