#include "service_broker.h"
#include "main_ui_channel.h"


void ServiceBroker::Start(void) {

    channel_ = new MainUIChannel();

    httpd_.BindLowerLayer(this);
    BindUpperLayer(&httpd_);
    BindLowerLayer(channel_);
    channel_->BindUpperLayer(this);

    httpd_.Start();
}

void ServiceBroker::Stop(void) {

    httpd_.Stop();

    BindLowerLayer(0);
    BindUpperLayer(0);
    delete channel_;

}

void ServiceBroker::OnRequest() {
    // TODO(ghilbut): JS call
    FireRequest();
}

void ServiceBroker::OnResponse() {
    // TODO(ghilbut): JS call
    FireResponse();
}

void ServiceBroker::OnNotify() {
    // TODO(ghilbut): JS call
    FireNotify();
}

void ServiceBroker::OnEvent() {
    // TODO(ghilbut): JS call
    FireEvent();
}
