#include "main_ui_channel.h"


void MainUIChannel::OnRequest() {
    FireResponse();
}

void MainUIChannel::OnResponse() {
    // nothing
}

void MainUIChannel::OnNotify() {
    FireEvent();
}

void MainUIChannel::OnEvent() {
    // nothing
}
