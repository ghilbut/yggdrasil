#include "ctrl_point.h"

#include "service/service_finder.h"
//#include "uart_server.h"
#include "tcp_server.h"
#include "http/http_server.h"
#include "service/service.h"
#include "service/service_info.h"
#include <mongoose/mongoose.h>
#include <json/json.h>
#include <boost/filesystem.hpp>


CtrlPoint::CtrlPoint(const std::string& document_root
                     , HttpServer& httpd
                     , DevicePool& devices
                     , ServicePool& services)
    : HttpObject(document_root, 80)
    , httpd_(httpd)
    , devices_(devices)
    , services_(services)
    , finder_(0) {
    // nothing
}

CtrlPoint::~CtrlPoint(void) {
    // nothing
}

bool CtrlPoint::Start(void) {

    if (!httpd_.Insert(this)) {
        return false;
    }
    thread_.swap(boost::thread(boost::bind(&CtrlPoint::thread_main, this)));
    thread_.detach();
    return true;
}

void CtrlPoint::Stop(void) {
    io_service_.stop();
}

void CtrlPoint::thread_main(void) {
    
    TcpServer t(io_service_, 8070);
    t.BindHandleConnect(boost::bind(&CtrlPoint::OnConnected, this, _1, _2));
    //UartServer z(io_service_, "port");

    ServiceFinder sf(io_service_, 8071);
    ServicePool::Iterator itr = services_.Begin();
    ServicePool::Iterator end = services_.End();
    for (; itr != end; ++itr) {
        const std::string& id = itr->first;
        sf.RegistTarget(id.c_str());
    }
    finder_ = &sf;

    services_.Start();
    io_service_.run();
    services_.Stop();

    Json::Value root(Json::objectValue);
    root["event"] = "Disconnected";
    Json::FastWriter writer;
    const std::string json = writer.write(root);
    FireEvent(json);

    finder_ = 0;
    t.UnbindHandleConnect();
}

bool CtrlPoint::DoRequest(mg_connection* conn
                          , const char* method
                          , const char* query) {

    if (strcmp(query, "get/device/list") == 0) {

        Json::Value root(Json::arrayValue);
        ServicePool::Iterator itr = services_.Begin();
        ServicePool::Iterator end = services_.End();
        for (; itr != end; ++itr) {
            const Service* s = itr->second;
            const ServiceInfo& info = s->info();
            Json::Value item(Json::objectValue);
            item["id"] = info.id();
            item["device"] = info.device();
            item["nickname"] = info.nickname();
            item["port"] = s->port();
            item["disabled"] = (connecting_list_.find(info.id()) == connecting_list_.end());
            root.append(item);
        }
        Json::FastWriter writer;
        const std::string json = writer.write(root);

        const char* response = ""
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"        // Always set Content-Length
            "\r\n"
            "%s";
        const int result = mg_printf(conn, response, json.length(), json.c_str());
        if (result > 0) {
            return true;
        }

        if (result == 0) {
            // TODO(jh81.kim):
            // connection has been closed
        }

        if (result < 0) {
            // TODO(jh81.kim):
            // error
        }

        return false;
    }

    return false;
}

bool CtrlPoint::DoExecute(mg_connection* conn
                          , const char* method
                          , const char* uri) {
    return false;
}

void CtrlPoint::OnConnected(const std::string& json, Channel::Ptr channel) {

    Json::Value root;
    Json::Reader reader;
    reader.parse(json.c_str(), root);

    const std::string& id = root["id"].asString();
    printf(" id: %s\n device: %s\n nickname: %s\n protocol: %s\n\n"
        , id.c_str()
        , root["device"].asCString()
        , root["nickname"].asCString()
        , root["protocol"].asCString());

    Service* s = services_[id.c_str()];
    // TODO(jh81.kim):
    // I'm worryed about channel's life sycle
    s->BindHandleDisconnected(boost::bind(&CtrlPoint::OnDisonnected, this, _1));
    s->BindChannel(channel);

    connecting_list_.insert(id);
    finder_->UnregistTarget(id.c_str());

    {
        Json::Value root(Json::objectValue);
        root["event"] = "ServiceAdded";
        root["id"] = id;

        Json::FastWriter writer;
        const std::string json = writer.write(root);
        FireEvent(json);
    }
}

void CtrlPoint::OnDisonnected(const char* id) {

    finder_->RegistTarget(id);
    connecting_list_.erase(id);

    Service* s = services_[id];
    s->UnbindChannel();

    {
        Json::Value root(Json::objectValue);
        root["event"] = "ServiceRemoved";
        root["id"] = id;

        Json::FastWriter writer;
        const std::string json = writer.write(root);
        FireEvent(json);
    }
}