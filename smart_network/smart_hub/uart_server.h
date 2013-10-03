#ifndef UART_SERVER_H_
#define UART_SERVER_H_

#include "zigbee_driver.h"


class UartServer {
public:
    UartServer(boost::asio::io_service& io_service, const char* port);
    ~UartServer(void);

private:
    ZigbeeDriver driver_;
};


#endif  // ZIGBEE_SERVER_H_