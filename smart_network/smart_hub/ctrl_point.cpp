#include "ctrl_point.h"
#include "http/http_fwd.h"
//#include "uart_server.h"
#include "tcp_server.h"
#include "service_proxy.h"
#include "service/service.h"
#include "service/service_desc.h"
#include "codebase/ssdp.h"
#include <mongoose/mongoose.h>
#include <json/json.h>



CtrlPoint::CtrlPoint(const std::string& storage_root)
    : ssdp_sender_(0)
    , ws_ping_scheduler_(io_service_)
    , common_root_((boost::filesystem::path(storage_root) / "common").string())
    
    , device_manager_((boost::filesystem::path(storage_root) / "devices").string())
    , service_factory_(device_manager_, (boost::filesystem::path(storage_root) / "services").string()) 
    , main_ui_service_((boost::filesystem::path(storage_root) / "main").string(), 80) {
    // nothing
}

CtrlPoint::~CtrlPoint(void) {
    // nothing
}

bool CtrlPoint::Start(void) {

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

    SsdpSender ss(io_service_);
    ServiceFactory::Iterator itr = service_factory_.Begin();
    ServiceFactory::Iterator end = service_factory_.End();
    for (; itr != end; ++itr) {
        const std::string& id = itr->first;
        ss.RegistTarget(id.c_str());
    }
    ssdp_sender_ = &ss;

    
    

    //service_factory_.Start();


    ws_ping_scheduler_.Register(&main_ui_service_);
    main_ui_service_.BindUIHandler(this);
    main_ui_service_.BindCommonPathHandler(boost::bind(&CtrlPoint::HandleCommonPath, this, _1, _2));




    ws_ping_scheduler_.Start();
    main_ui_service_.Start();

    io_service_.run();

    main_ui_service_.Stop();
    ws_ping_scheduler_.Stop();



    main_ui_service_.UnbindCommonPathHandler();
    main_ui_service_.UnbindUIHandler();
    ws_ping_scheduler_.Unregister(&main_ui_service_);



    //service_factory_.Stop();

    
    

    Json::Value root(Json::objectValue);
    root["event"] = "Disconnected";
    Json::FastWriter writer;
    const std::string json = writer.write(root);
    //FireEvent(json);

    ssdp_sender_ = 0;
    t.UnbindHandleConnect();
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

    /*
    Service* s = service_factory_[id];
    // TODO(ghilbut):
    // I'm worryed about channel's life sycle
    s->BindHandleDisconnected(boost::bind(&CtrlPoint::OnDisonnected, this, _1));
    s->BindChannel(channel);
    */

    connecting_list_.insert(id);
    ssdp_sender_->UnregistTarget(id.c_str());

    {
        Json::Value root(Json::objectValue);
        root["event"] = "ServiceAdded";
        root["id"] = id;

        Json::FastWriter writer;
        const std::string json = writer.write(root);
        //FireEvent(json);
    }
}

void CtrlPoint::OnDisonnected(const char* id) {

    ssdp_sender_->RegistTarget(id);
    connecting_list_.erase(id);

    /*
    Service* s = service_factory_[id];
    s->UnbindChannel();
    */

    {
        Json::Value root(Json::objectValue);
        root["event"] = "ServiceRemoved";
        root["id"] = id;

        Json::FastWriter writer;
        const std::string json = writer.write(root);
        //FireEvent(json);
    }
}

void CtrlPoint::handle_get_device_list(std::string& json) {

    Json::Value root(Json::arrayValue);
    ServiceFactory::Iterator itr = service_factory_.Begin();
    ServiceFactory::Iterator end = service_factory_.End();
    for (; itr != end; ++itr) {
        const ServiceProxy* s = itr->second;
        const ServiceDesc& info = s->desc();
        Json::Value item(Json::objectValue);
        item["id"] = info.id();
        item["device"] = info.device();
        item["nickname"] = info.nickname();
        item["port"] = s->port();
        item["disabled"] = (connecting_list_.find(info.id()) == connecting_list_.end());
        root.append(item);
    }
    Json::FastWriter writer;
    json = writer.write(root);
}

bool CtrlPoint::HandleCommonPath(const char* uri, std::string& filepath) {
    const boost::filesystem::path root(common_root_);
    filepath = (root / (uri + kCommonKeySize)).string();
    return boost::filesystem::is_regular_file(filepath);
}