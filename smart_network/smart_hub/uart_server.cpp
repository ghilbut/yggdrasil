#include "uart_server.h"

UartServer::UartServer(boost::asio::io_service& io_service, const char* port)
    : driver_(io_service, port) {

}

UartServer::~UartServer(void) {

}
