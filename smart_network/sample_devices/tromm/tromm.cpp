#include "tromm.h"
#include "message.h"
#include "codebase/utility.h"
#include <json/json.h>


Tromm::Tromm(boost::asio::io_service& io_service, const std::string& description)
    : Device(io_service, description)
    , is_power_on_(false) 
    , is_running_(false) 
    , mode_(kMode00) {
    // nothing
}

Tromm::~Tromm(void) {
    // nothing
}

void Tromm::OnConnected(Event& event) {
    // nothing
}

void Tromm::OnRequest(const Request& req, Response& res) {

    const std::string& query = req.query();

    if (query == "get/status") {
        res.SetParam("isPowerOn", is_power_on_);
        res.SetParam("isRunning", is_running_);
        res.SetParam("mode", mode_);
        return;
    }
}

void Tromm::OnNotify(const std::string& text) {

    Json::Value root(Json::objectValue);
    Json::Reader reader;
    if (::ReadJson(text, root)) {

        const Json::Value& name = root["name"];
        if (name.isString() && name.asString() == "statusChanged") {
            Json::Value params = root["params"];

            Json::Value& ipo = params["isPowerOn"];
            if (ipo.isBool()) {
                is_power_on_ = ipo.asBool();
            }
            Json::Value& ir = params["isRunning"];
            if (ir.isBool()) {
                is_running_ = ir.asBool();
            }
            Json::Value& m = params["mode"];
            if (m.isInt()) {
                mode_ = static_cast<Mode>(m.asInt());
            }

            printf("\n================================\n");
            printf(  "==       status changed       ==\n\n");
            printf(  "    power  : %s\n", is_power_on_ ? "on"      : "off");
            printf(  "    status : %s\n", is_running_  ? "started" : "stoped");
            printf(  "    mode   : %d\n", mode_);
            printf("\n================================\n\n");
        }
    }
    FireEvent(text);
}

void Tromm::OnError(void) {
    // TODO(jh81.kim): so what ?
}

void Tromm::OnDisconnected(void) {
    // TODO(jh81.kim): so what ?
}
