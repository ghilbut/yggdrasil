#include "localbox.h"
#include "sample.h"
#include "basebox/service.h"
#include "basebox/service_ref.h"
#include "basebox/service_manager.h"
#include "base/io_service.h"
#include "backend/network_manager.h"
#include "backend/tcp_adapter.h"
#include <json/json.h>
#include <cassert>


static int http_request_handler(struct mg_connection* conn, enum mg_event ev) {
    LocalBox* s = static_cast<LocalBox*>(conn->server_param);
    return s->HttpRequest(conn, ev);
};

LocalBox::LocalBox(const char* rootpath) 
    : isolate_(v8::Isolate::GetCurrent())
    , handle_scope_(isolate_) {

    //isolate_->SetData(0, &io_service_);

    v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate_);
    global->Set(v8::String::NewFromUtf8(isolate_, "print"), v8::FunctionTemplate::New(isolate_, Print));
    global->Set(v8::String::NewFromUtf8(isolate_, "read"), v8::FunctionTemplate::New(isolate_, Read));
    global->Set(v8::String::NewFromUtf8(isolate_, "load"), v8::FunctionTemplate::New(isolate_, Load));
    global->Set(v8::String::NewFromUtf8(isolate_, "quit"), v8::FunctionTemplate::New(isolate_, Quit));
    global->Set(v8::String::NewFromUtf8(isolate_, "version"), v8::FunctionTemplate::New(isolate_, Version));

    /*global->Set(
        v8::String::NewFromUtf8(isolate_, "Http")
        , Http::Template::New(isolate_)
        , Http::kAttribute);*/


    v8::Local<v8::Context> context = v8::Context::New(isolate_, NULL, global);

    
    
    context->Enter();



    BindSample(context);
    /*{
        v8::HandleScope handle_scope(context->GetIsolate());
        context->Global()->Set(
            v8::String::NewFromUtf8(isolate_, "http")
            , Http::NewInstance(isolate_)
            , Http::kAttribute);
    }*/

    context_ = context;



    
    boost::filesystem::path device_root(rootpath);
    device_root /= "devices";
    if (!boost::filesystem::exists(device_root)) {
        // TODO(ghilbut): error handling
    }

    boost::filesystem::path service_root(rootpath);
    service_root /= "services";
    if (!boost::filesystem::exists(service_root)) {
        // TODO(ghilbut): error handling
    }





    net_manager_ = new NetworkManager(io_service_->IO());
    NetworkAdapter::Ptr eth_adapter(new TcpAdapter(io_service_, this, 8070));
    net_manager_->Register("ethernet", eth_adapter);
    net_manager_->Start();

    device_manager_ = new DeviceManager(io_service_, device_root.string());
    service_manager_ = new ServiceManager(*device_manager_, *this, service_root.string());
}

LocalBox::~LocalBox(void) {

    context_->Exit();

    if (service_manager_) {
        delete service_manager_;
    }
    if (device_manager_) {
        delete device_manager_;
    }
    if (net_manager_) {
        net_manager_->Stop();
        delete net_manager_;
    }
}

void LocalBox::RunShell(void) {

    ServiceRef s = (*service_manager_)["a"];
    if (!s.IsNull()) {
        s->RunShell();
    }
}





void LocalBox::OnServiceOpen(const ServiceRef& service) {
    const char* id = service->id();
    servers_.Create(id, boost::bind(&Service::HttpRequest, service.Get(), _1, _2));
    net_manager_->RegisterSsdpTarget("ethernet", id);
}

void LocalBox::OnServiceClosed(const ServiceRef& service) {
    servers_.Destroy(service->id());
}

void LocalBox::OnChannelOpen(const ChannelRef& channel, const std::string& text) {

    Json::Value root;
    Json::Reader reader;
    reader.parse(text.c_str(), root);

    const std::string& id = root["id"].asString();
    printf(" id: %s\n device: %s\n nickname: %s\n protocol: %s\n\n"
        , id.c_str()
        , root["device"].asCString()
        , root["nickname"].asCString()
        , root["protocol"].asCString());

    ServiceRef s = (*service_manager_)[id];
    s->BindChannel(channel);

    /*ServiceBroker* s = service_factory_[id];
    // TODO(ghilbut):
    // I'm worryed about channel's life cycle
    {
        // TODO(ghilbut):
        // if service proxy created here bind handlers
        // or, it is not necessary.
        // s->BindCommonPathHandler(boost::bind(&CtrlPoint::handle_get_common_path, this, _1, _2));
        // s->BindDisconnectedHandler(boost::bind(&CtrlPoint::handle_disconnected, this, _1));
    }
    s->BindChannel(channel);*/

    //connecting_list_.insert(id);
    net_manager_->UnregisterSsdpTarget("ethernet", id);

    /*{
        Json::Value root(Json::objectValue);
        root["event"] = "ServiceAdded";
        root["id"] = id;

        Json::FastWriter writer;
        const std::string json = writer.write(root);
        main_ui_service_.FireEvent(json);
    }*/
}

void LocalBox::OnChannelRecv(const ChannelRef& channel, const std::string& text) {
    assert(false);
}

void LocalBox::OnChannelClosed(const ChannelRef& channel) {
    assert(false);
}
