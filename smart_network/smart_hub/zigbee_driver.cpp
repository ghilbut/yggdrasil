#include "zigbee_driver.h"

ZigbeeDriver::ZigbeeDriver(boost::asio::io_service& io_service, const char* port)
        : io_service_(io_service), serial_(io_service, port) {
    static const unsigned int kBaudrate = 9600;
    serial_.set_option(boost::asio::serial_port_base::baud_rate(kBaudrate));
}

ZigbeeDriver::~ZigbeeDriver(void) {

}
