#ifndef ZIGBEE_DRIVER_H_
#define ZIGBEE_DRIVER_H_

#include <boost/asio.hpp>

class ZigbeeDriver {
public:
    ZigbeeDriver(boost::asio::io_service& io_service, const char* port);
    ~ZigbeeDriver(void);

private:
    boost::asio::io_service& io_service_;
    boost::asio::serial_port serial_;
};


#endif  // ZIGBEE_DRIVER_H_