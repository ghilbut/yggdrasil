#include "tromm.h"
#include "message.h"


Tromm::Tromm(boost::asio::io_service& io_service, const std::string& description)
    : Device(io_service, description)
    , is_power_on_(false) 
    , is_running_(false) 
    , mode_(kMode01) {
    // nothing
}

Tromm::~Tromm(void) {

}

void Tromm::OnConnected(Event& event) {

}

void Tromm::OnRequest(const Request& req, Response& res) {

    if (req.query() == "get/status") {
        res.SetParam("isPowerOn", is_power_on_);
        res.SetParam("isRunning", is_running_);
        res.SetParam("mode", mode_);
    }
}

void Tromm::OnError(void) {

}

void Tromm::OnDisconnected(void) {

}
