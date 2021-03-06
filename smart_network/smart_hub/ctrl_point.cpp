#include "ctrl_point.h"
#include "codebase/storage_path.h"
#include "codebase/tcp_adapter.h"
#include "service_broker.h"
#include "service_desc.h"
#include <mongoose/mongoose.h>
#include <json/json.h>



CtrlPoint::CtrlPoint(const StoragePath& storage)
    : common_root_(storage.common_root())
    , device_manager_(storage.device_root())
    , main_ui_service_(storage.main_ui_root(), 80)
    , service_factory_(device_manager_, storage.service_root()) 
    // , ws_ping_scheduler_(io_service_)
    , net_manager_(io_service_) {
    // nothing
}

CtrlPoint::~CtrlPoint(void) {
    if (!io_service_.stopped()) {
        Stop();
    }
}

void CtrlPoint::OnConnected(const std::string& json, Channel* channel) {

    Json::Value root;
    Json::Reader reader;
    reader.parse(json.c_str(), root);

    const std::string& id = root["id"].asString();
    printf(" id: %s\n device: %s\n nickname: %s\n protocol: %s\n\n"
        , id.c_str()
        , root["device"].asCString()
        , root["nickname"].asCString()
        , root["protocol"].asCString());

    ServiceBroker* s = service_factory_[id];
    // TODO(ghilbut):
    // I'm worryed about channel's life sycle
    {
        // TODO(ghilbut):
        // if service proxy created here bind handlers
        // or, it is not necessary.
        // s->BindCommonPathHandler(boost::bind(&CtrlPoint::handle_get_common_path, this, _1, _2));
        // s->BindDisconnectedHandler(boost::bind(&CtrlPoint::handle_disconnected, this, _1));
    }
    s->BindChannel(channel);

    connecting_list_.insert(id);
    net_manager_.UnregisterSsdpTarget("ethernet", id);

    {
        Json::Value root(Json::objectValue);
        root["event"] = "ServiceAdded";
        root["id"] = id;

        Json::FastWriter writer;
        const std::string json = writer.write(root);
        main_ui_service_.FireEvent(json);
    }
}

void CtrlPoint::OnResponse(const std::string& json) {
    BOOST_ASSERT(false);
}

void CtrlPoint::OnEvent(const std::string& text) {
    BOOST_ASSERT(false);
}

void CtrlPoint::OnDisconnected(void) {
    BOOST_ASSERT(false);
}

bool CtrlPoint::Start(void) {
    thread_.swap(boost::thread(boost::bind(&CtrlPoint::thread_main, this)));
    return true;
}

void CtrlPoint::Stop(void) {
    io_service_.stop();
    thread_.join();
}

void CtrlPoint::thread_main(void) {

    NetworkAdapter::Ptr eth_adapter(new TcpAdapter(io_service_, this, 8070));
    net_manager_.Register("ethernet", eth_adapter);
    net_manager_.Start();



    ServiceFactory::Iterator itr = service_factory_.Begin();
    ServiceFactory::Iterator end = service_factory_.End();
    for (; itr != end; ++itr) {
        const std::string& id = itr->first;
        ServiceBroker* service = itr->second;
        service->BindCommonPathHandler(boost::bind(&CtrlPoint::handle_get_common_path, this, _1, _2));
        service->BindDisconnectedHandler(boost::bind(&CtrlPoint::handle_disconnected, this, _1));
        net_manager_.RegisterSsdpTarget("ethernet", id);
    }



    // ws_ping_scheduler_.Register(&main_ui_service_);
    main_ui_service_.BindUIHandler(this);
    main_ui_service_.BindCommonPathHandler(boost::bind(&CtrlPoint::handle_get_common_path, this, _1, _2));

    // ws_ping_scheduler_.Start();
    main_ui_service_.Start();
    service_factory_.StartAll();

    io_service_.run();

    service_factory_.StopAll();
    main_ui_service_.Stop();
    // ws_ping_scheduler_.Stop();

    main_ui_service_.UnbindCommonPathHandler();
    main_ui_service_.UnbindUIHandler();
    // ws_ping_scheduler_.Unregister(&main_ui_service_);



    Json::Value root(Json::objectValue);
    root["event"] = "Disconnected";
    Json::FastWriter writer;
    const std::string json = writer.write(root);
    main_ui_service_.FireEvent(json);

    net_manager_.Stop();
}

void CtrlPoint::handle_get_device_list(std::string& json) {

    Json::Value root(Json::arrayValue);
    ServiceFactory::Iterator itr = service_factory_.Begin();
    ServiceFactory::Iterator end = service_factory_.End();
    for (; itr != end; ++itr) {
        const ServiceBroker* s = itr->second;
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

bool CtrlPoint::handle_get_common_path(const char* uri, std::string& filepath) {
    const boost::filesystem::path root(common_root_);
    filepath = (root / uri).string();
    return boost::filesystem::is_regular_file(filepath);
}

void CtrlPoint::handle_disconnected(const std::string& id) {

    net_manager_.RegisterSsdpTarget("ethernet", id);
    connecting_list_.erase(id);

    Json::Value root(Json::objectValue);
    root["event"] = "ServiceRemoved";
    root["id"] = id;

    Json::FastWriter writer;
    const std::string json = writer.write(root);
    main_ui_service_.FireEvent(json);
}